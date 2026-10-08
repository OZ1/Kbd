using System.Globalization;
using System.Runtime.CompilerServices;

namespace KbdImage.Model;

using static Char;
using static UnicodeCategory;
using static KbdMod;
using static WCH;

public enum Slot
{
	Normal,
	Shift,
	Ctrl,
	Kana,
	KanaShift,
	Caps,
	CapsShift,
}

/// <summary>Видимая ячейка кнопки. Boxed — непечатный символъ (рисуется сокращеніемъ въ пунктирной рамкѣ), Dead — мёртвая клавиша.</summary>
public readonly record struct Cell(Slot Slot, string Text, bool Dead, bool Boxed);

public static class Cells
{
	static readonly KbdMod[] SlotBits = [0, Shift, Ctrl, Kana, Kana | Shift];

	static readonly string[] C0 =
	[
		"NUL", "SOH", "STX", "ETX", "EOT", "ENQ", "ACK", "BEL", "BS", "HT", "LF", "VT", "FF", "CR", "SO", "SI",
		"DLE", "DC1", "DC2", "DC3", "DC4", "NAK", "SYN", "ETB", "CAN", "EM", "SUB", "ESC", "FS", "GS", "RS", "US",
	];

	static readonly Dictionary<char, string> Names = new()
	{
		{      ' ', "SP"     }, {   '\x7F', "DEL"    }, {   '\x85', "NEL"    }, {   '\xA0', "NB\nSP" }, {   '\xAD', "SHY"    },
		{ '\u034F', "CGJ"    }, { '\u2000', "EN\nQD" }, { '\u2001', "EM\nQD" }, { '\u2002', "EN\nSP" }, { '\u2003', "EM\nSP" },
		{ '\u2004', "3/\nEM" }, { '\u2005', "4/\nEM" }, { '\u2006', "6/\nEM" }, { '\u2007', "FSP"    }, { '\u2008', "PSP"    },
		{ '\u2009', "TH\nSP" }, { '\u200A', "HSP"    }, { '\u200B', "ZW\nSP" }, { '\u200C', "ZW\nNJ" }, { '\u200D', "ZWJ"    },
		{ '\u200E', "LRM"    }, { '\u200F', "RLM"    }, { '\u2028', "LS\nEP" }, { '\u2029', "PS\nEP" }, { '\u202F', "NN\nSP" },
		{ '\u205F', "MM\nSP" }, { '\u2060', "WJ"     }, { '\u2061', "FA"     }, { '\u2062', "IT"     }, { '\u2063', "IS"     },
		{ '\u2064', "IP"     }, { '\u3000', "ID\nSP" }, { '\uFEFF', "BOM"    },
	};

	/// <summary>Всѣ видимыя ячейки кнопки съ учётомъ настроекъ отображенія KbdEdit.</summary>
	public static List<Cell> Resolve(KBDTABLES layout, int vk)
	{
		List<Cell> result = [];
		if (!layout.Rows.TryGetValue(vk, out VK_TO_WCHARS? row))
			return result;

		Span<char> raw = stackalloc char[7];
		for (int s = 0; s < 5; s++)
			raw[s] = layout.GetMain(row, SlotBits[s]);
		// Цифры цифрового блока (KBDNUMPAD|KBDSPECIAL): Shift даётъ Home/↑/…, а не символъ раскладки — какъ въ KbdEdit
		if (IsNumpadDigit(vk))
			raw[(int)Slot.Shift] = WCH_NONE;

		char caps;
		char capsShift;
		if (row.Attr.HasFlag(Attr.SgCaps))
		{
			caps      = layout.GetExtra(row, 0);
			capsShift = layout.GetExtra(row, Shift);
		}
		else if (row.Attr.HasFlag(Attr.CapLok))
		{
			caps      = raw[(int)Slot.Shift];
			capsShift = raw[(int)Slot.Normal];
		}
		else
		{
			caps      = raw[(int)Slot.Normal];
			capsShift = raw[(int)Slot.Shift];
		}
		raw[(int)Slot.Caps     ] = caps;
		raw[(int)Slot.CapsShift] = capsShift;

		// Tab и BackSpace — свои собственные символы на кнопкѣ не печатаются
		if (vk is '\b' or '\t')
		{
			for (int s = 0; s < raw.Length; s++)
				if (raw[s] is '\b' or '\t')
					raw[s] = WCH_NONE;
		}

		Span<bool> dead = stackalloc bool[7];
		Buffer7<string?> textBuf = default;
		Span<string?> text = textBuf;
		for (int s = 0; s < 7; s++)
		{
			char v = raw[s];
			if (v is WCH_NONE or WCH_LGTR) continue;
			if (v == WCH_DEAD)
			{
				// Знакъ мёртвой клавиши лежитъ въ строкѣ VK__none_ въ томъ же столбцѣ
				if (s >= 5)
					continue;
				char g = layout.GetExtra(row, SlotBits[s]);
				text[s] = g is WCH_NONE or WCH_DEAD or WCH_LGTR ? "◌" : Describe(g);
				dead[s] = true;
				continue;
			}
			text[s] = Describe(v);
		}

		Span<bool> hide = stackalloc bool[7];
		static bool Same(ReadOnlySpan<string?> text, ReadOnlySpan<bool> dead, Slot a, Slot b)
			=> text[(int)a] != null &&
			   text[(int)a] == text[(int)b] &&
			   dead[(int)a] == dead[(int)b];

		// Одинаковый знакъ безъ Shift и съ Shift рисуется одинъ разъ — на обычномъ мѣстѣ
		foreach ((Slot n, Slot sh) in (ReadOnlySpan<(Slot, Slot)>)[(Slot.Normal, Slot.Shift), (Slot.Kana, Slot.KanaShift), (Slot.Caps, Slot.CapsShift)])
			if (Same(text, dead, n, sh)) hide[(int)sh] = true;
			// If UPPERCASE equivalent mapped on Shift: show only UPPERCASE
			else if (IsUpperOf(raw[(int)n], raw[(int)sh]))
				hide[(int)n] = true;

		// Caps on: show only on Caps off; Kana: show only on non-Kana — повторъ того, что уже есть безъ Caps / безъ Kana, не рисуется
		foreach (Slot c in (ReadOnlySpan<Slot>)[Slot.Caps, Slot.CapsShift, Slot.Kana, Slot.KanaShift])
			if (Same(text, dead, c, Slot.Normal) || Same(text, dead, c, Slot.Shift))
				hide[(int)c] = true;

		// Одинаковый символъ въ разныхъ слотахъ рисуется одинъ разъ (первый по порядку слотовъ)
		List<(string, bool)> shown = [];
		for (int s = 0; s < 7; s++)
		{
			if (text[s] == null || hide[s]) continue;
			if (shown.Contains((text[s]!, dead[s]))) hide[s] = true;
			else shown.Add((text[s]!, dead[s]));
		}

		for (int s = 0; s < 7; s++)
			if (text[s] != null && !hide[s])
				result.Add(new Cell((Slot)s, text[s]!, dead[s], !dead[s] && NeedsBox(raw[s])));
		return result;
	}

	static bool IsNumpadDigit(int vk) => vk is >= 0x60 and <= 0x69 or 0x6E;

	static bool IsUpperOf(char lower, char upper)
	{
		if (lower >= WCH_NONE || upper >= WCH_NONE || lower == upper) return false;
		return ToUpperInvariant(lower) == upper && upper != lower;
	}

	static bool NeedsBox(char code)
	{
		if (code is < ' ' or '\x7F' or >= '\x80' and < '\xA0') return true;
		if (Names.ContainsKey(code)) return true;
		if (IsWhiteSpace(code)) return true;
		return GetUnicodeCategory(code) is Format or PrivateUse or OtherNotAssigned;
	}

	/// <summary>Текстъ ячейки: символъ, сокращеніе непечатнаго или «◌» + знакъ для комбинирующихъ.</summary>
	static string Describe(char code)
	{
		bool boxed = NeedsBox(code);
		if (code < ' ') return C0[code];
		if (Names.TryGetValue(code, out string? name))
			return name;
		if (boxed)
		{
			string h = ((int)code).ToString("X4", CultureInfo.InvariantCulture);
			return h[..2] + '\n' + h[2..];
		}
		if (GetUnicodeCategory(code) is NonSpacingMark or EnclosingMark or SpacingCombiningMark)
			return "◌" + code;
		return code.ToString();
	}

	/// <summary>Отличаются ли обычный символъ или Shift-символъ кнопки отъ сѵстемной (US) разкраски.</summary>
	public static bool DiffersFromSystem(KBDTABLES layout, ushort scan, bool ext, int vk)
	{
		if (vk is '\b' or '\t') return false;
		if (!SystemLayout.For(layout.LCID).TryGet(scan, ext, out (char Normal, char Shift) sys)) return false;
		char n = '\0';
		char s = '\0';
		if (layout.Rows.TryGetValue(vk, out VK_TO_WCHARS? row))
		{
			n = layout.GetMain(row, 0);
			s = IsNumpadDigit(vk) ? '\0' : layout.GetMain(row, Shift);
			if (n >= WCH_NONE) n = '\0';
			if (s >= WCH_NONE) s = '\0';
		}
		return n != sys.Normal || s != sys.Shift;
	}
}

/// <summary>Семь мѣстъ на стекѣ — для ссылочныхъ тѵповъ, которыхъ stackalloc не беретъ.</summary>
[InlineArray(7)]
file struct Buffer7<T>
{
	T _e0;
}
