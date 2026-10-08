using System.Text;
using System.Globalization;

namespace KbdImage.Parsing;

using Model;

using static File;
using static Int32;
using static NumberStyles;
using static StringComparison;

using static Model.WCH;
using static Model.KBDTABLES;

/// <summary>Читаетъ таблицы разкладки (ausVK, CharModifiers, VK_TO_WCHARSn) изъ MyKbd*.cpp.</summary>
public static class CppLayoutReader
{
	const int Unknown = -1;

	/// <summary>Читаетъ .cpp; скан-коды, не переопредѣлённые черезъ #define Tnn, — изъ сѵстемной разкладки языка lcid.</summary>
	public static KBDTABLES Read(string path, ushort lcid) => Parse(DecodeText(ReadAllBytes(path)), lcid);

	static string DecodeText(byte[] bytes)
	{
		int skip = bytes.Length >= 3 && bytes[0] == 0xEF && bytes[1] == 0xBB && bytes[2] == 0xBF ? 3 : 0;
		try
		{
			return new UTF8Encoding(false, true).GetString(bytes, skip, bytes.Length - skip);
		}
		catch (DecoderFallbackException)
		{
			Encoding.RegisterProvider(CodePagesEncodingProvider.Instance);
			return Encoding.GetEncoding(1251).GetString(bytes);
		}
	}

	static KBDTABLES Parse(string source, ushort lcid)
	{
		CppLexer lx = CppLexer.Lex(source);
		KBDTABLES layout = new() { LCID = lcid };
		VkDefaults.Fill(layout, lcid);

		foreach (KeyValuePair<string, List<Tok>> d in lx.Defines)
		{
			// #define T47 VK_... — переопредѣленіе скан-кода
			if (d.Key.Length == 3 && d.Key[0] == 'T' && TryParse(d.Key[1..], HexNumber, null, out int sc) && sc < 0x80)
				layout.ScanToVk[sc] = Eval(lx, d.Value, 0);
		}

		List<Tok> t = lx.Tokens;
		for (int i = 0; i < t.Count; i++)
		{
			if (t[i].Kind != TokKind.Ident)
				continue;
			if (t[i].Text == "CharModifiers")
				i = ReadModifiers(lx, layout, i);
			else if (t[i].Text.StartsWith("VK_TO_WCHARS", Ordinal) && i + 4 < t.Count
				&& t[i + 1].Kind == TokKind.Ident && t[i + 2].Text == "[" && t[i + 3].Text == "]" && t[i + 4].Text == "=")
				i = ReadTable(lx, layout, i + 5);
		}
		return layout;
	}

	static int ReadModifiers(CppLexer lx, KBDTABLES layout, int i)
	{
		List<Tok> t = lx.Tokens;
		// CharModifiers = { &aVkToBits[0], N, { v, v, ... } }
		while (i < t.Count && !(t[i].Text == "{" && i > 0 && t[i - 1].Text == "="))
			i++;
		int inner = -1;
		for (int j = i + 1; j < t.Count && inner < 0; j++)
			if (t[j].Text == "{")
				inner = j;
			else if (t[j].Text == "}")
				break;
		if (inner < 0)
			return i;
		List<List<Tok>> items = SplitBraced(t, ref inner);
		List<int> mods = [];
		foreach (List<Tok> item in items)
			mods.Add(item.Count == 1 && item[0].Text == "SHFT_INVALID" ? SHFT_INVALID : Eval(lx, item, 0));
		if (mods.Count > 0)
			layout.ModNumber = [.. mods];
		return inner;
	}

	static int ReadTable(CppLexer lx, KBDTABLES layout, int i)
	{
		List<Tok> t = lx.Tokens;
		if (i >= t.Count || t[i].Text != "{")
			return i;
		i++;
		VK_TO_WCHARS? last = null;
		while (i < t.Count && t[i].Text != "}")
		{
			if (t[i].Text == "{")
			{
				List<List<Tok>> e = SplitBraced(t, ref i);
				if (e.Count >= 2)
				{
					int vk = Eval(lx, e[0], 0);
					int attr = Max(0, Eval(lx, e[1], 0));
					List<char> wch = [];
					for (int n = 2; n < e.Count; n++)
					{
						int v = Eval(lx, e[n], 0);
						wch.Add(v == Unknown ? WCH_NONE : (char)v);
					}
					if (vk == 0xFF && last != null)
						last.Extra = [.. wch];
					else if (vk != Unknown)
					{
						last = new VK_TO_WCHARS { Attr = (Attr)attr, Main = [.. wch] };
						layout.Rows.TryAdd(vk, last);
					}
				}
			}
			i++;
		}
		return i;
	}

	/// <summary>Съ позиціи «{» собираетъ элементы черезъ запятую до парной «}»; i остаётся на «}».</summary>
	static List<List<Tok>> SplitBraced(List<Tok> t, ref int i)
	{
		List<List<Tok>> items = [];
		List<Tok> cur = [];
		int depth = 0;
		for (; i < t.Count; i++)
		{
			string p = t[i].Kind == TokKind.Punct ? t[i].Text : "";
			if (p is "{" or "(")
			{
				if (++depth == 1 && p == "{")
					continue;
			}
			else if (p is "}" or ")")
			{
				if (--depth == 0)
				{
					if (cur.Count > 0)
						items.Add(cur);
					return items;
				}
			}
			else if (p == "," && depth == 1)
			{
				items.Add(cur);
				cur = [];
				continue;
			}
			cur.Add(t[i]);
		}
		if (cur.Count > 0)
			items.Add(cur);
		return items;
	}

	/// <summary>Значеніе выраженія: числа, символы, имена (черезъ #define и kbd.h), «|».</summary>
	static int Eval(CppLexer lx, List<Tok> toks, int depth)
	{
		if (depth > 8 || toks.Count == 0)
			return Unknown;
		int result = 0;
		bool any = false;
		foreach (Tok k in toks)
		{
			int v;
			switch (k.Kind)
			{
				case TokKind.Num:
				case TokKind.Char:
					v = k.Value;
					break;
				case TokKind.Ident:
					if (lx.Defines.TryGetValue(k.Text, out List<Tok>? def))
						v = Eval(lx, def, depth + 1);
					else
						v = VkDefaults.Const(k.Text);
					break;
				default:
					continue;
			}
			if (v == Unknown)
				return Unknown;
			result |= v;
			any = true;
		}
		return any ? result : Unknown;
	}
}
