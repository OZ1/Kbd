using System.Globalization;
using System.Text;
using static System.Char;

namespace KbdImage.Parsing;

public enum TokKind
{
	Ident,
	Num,
	Char,
	Str,
	Punct,
}

public readonly record struct Tok(TokKind Kind, string Text, int Value);

/// <summary>Минимальный лексеръ C/C++: токены, #define без параметровъ; комментаріи и прочія директивы пропускаются.</summary>
public sealed class CppLexer
{
	public List<Tok> Tokens = [];
	public Dictionary<string, List<Tok>> Defines = [];

	public static CppLexer Lex(string src)
	{
		CppLexer lx = new();
		lx.Tokens = lx.Scan(src, true);
		return lx;
	}

	List<Tok> Scan(string s, bool directives)
	{
		List<Tok> t = [];
		int i = 0;
		bool lineStart = true;
		while (i < s.Length)
		{
			char c = s[i];
			if (c == '\n')
			{
				lineStart = true;
				i++;
			}
			else if (IsWhiteSpace(c))
				i++;
			else if (c == '/' && i + 1 < s.Length && s[i + 1] == '/')
			{
				while (i < s.Length && s[i] != '\n')
					i++;
			}
			else if (c == '/' && i + 1 < s.Length && s[i + 1] == '*')
			{
				int e = s.IndexOf("*/", i + 2, StringComparison.Ordinal);
				i = e < 0 ? s.Length : e + 2;
			}
			else if (c == '#' && directives && lineStart)
				i = Directive(s, i);
			else
			{
				lineStart = false;
				if (IsLetter(c) || c == '_')
				{
					int b = i;
					while (i < s.Length && (IsLetterOrDigit(s[i]) || s[i] == '_'))
						i++;
					string id = s[b..i];
					bool prefix = i < s.Length && (s[i] == '\'' || s[i] == '"') && id is "L" or "u" or "U" or "u8";
					if (!prefix)
						t.Add(new Tok(TokKind.Ident, id, 0));
				}
				else if (IsDigit(c))
				{
					int b = i;
					while (i < s.Length && (IsLetterOrDigit(s[i]) || s[i] == '_'))
						i++;
					t.Add(new Tok(TokKind.Num, s[b..i], ParseNumber(s[b..i])));
				}
				else if (c == '\'')
				{
					i++;
					int v = 0;
					while (i < s.Length && s[i] != '\'')
						v = ReadChar(s, ref i);
					i++;
					t.Add(new Tok(TokKind.Char, "", v));
				}
				else if (c == '"')
				{
					i++;
					while (i < s.Length && s[i] != '"')
						ReadChar(s, ref i);
					i++;
					t.Add(new Tok(TokKind.Str, "", 0));
				}
				else
				{
					t.Add(new Tok(TokKind.Punct, c.ToString(), c));
					i++;
				}
			}
		}
		return t;
	}

	/// <summary>Разбираетъ директиву; #define NAME value запоминаетъ. Возвращаетъ позицію послѣ строки.</summary>
	int Directive(string s, int i)
	{
		StringBuilder line = new();
		while (i < s.Length && s[i] != '\n')
		{
			if (s[i] == '\\' && i + 1 < s.Length && (s[i + 1] == '\n' || (s[i + 1] == '\r' && i + 2 < s.Length && s[i + 2] == '\n')))
			{
				i += s[i + 1] == '\r' ? 3 : 2;
				line.Append(' ');
				continue;
			}
			line.Append(s[i++]);
		}
		string d = line.ToString()[1..].TrimStart();
		if (d.StartsWith("define", StringComparison.Ordinal) && d.Length > 6 && IsWhiteSpace(d[6]))
		{
			string rest = d[6..].TrimStart();
			int n = 0;
			while (n < rest.Length && (IsLetterOrDigit(rest[n]) || rest[n] == '_'))
				n++;
			// Функціональные макросы (имя сразу передъ «(») не нужны
			if (n > 0 && !(n < rest.Length && rest[n] == '('))
				Defines[rest[..n]] = Scan(rest[n..], false);
		}
		return i;
	}

	static int ParseNumber(string s)
	{
		s = s.TrimEnd('u', 'U', 'l', 'L');
		try
		{
			if (s.StartsWith("0x", StringComparison.OrdinalIgnoreCase))
				return (int)Convert.ToUInt32(s[2..], 16);
			if (s.Length > 1 && s[0] == '0')
				return (int)Convert.ToUInt32(s, 8);
			return (int)uint.Parse(s, CultureInfo.InvariantCulture);
		}
		catch (Exception e) when (e is FormatException or OverflowException or ArgumentException)
		{
			return 0;
		}
	}

	static int ReadChar(string s, ref int i)
	{
		char c = s[i++];
		if (c != '\\' || i >= s.Length)
			return c;
		char e = s[i++];
		switch (e)
		{
			case 'n': return '\n';
			case 'r': return '\r';
			case 't': return '\t';
			case 'b': return '\b';
			case 'a': return '\a';
			case 'f': return '\f';
			case 'v': return '\v';
			case 'x':
			case 'u':
			case 'U':
			{
				int v = 0;
				int max = e == 'x' ? 8 : e == 'u' ? 4 : 8;
				for (int n = 0; n < max && i < s.Length && IsAsciiHexDigit(s[i]); n++)
					v = v * 16 + Convert.ToInt32(s[i++].ToString(), 16);
				return v;
			}
			case >= '0' and <= '7':
			{
				int v = e - '0';
				for (int n = 0; n < 2 && i < s.Length && s[i] is >= '0' and <= '7'; n++)
					v = v * 8 + (s[i++] - '0');
				return v;
			}
			default: return e;
		}
	}
}
