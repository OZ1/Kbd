namespace KbdImage.Model;

using static Enumerable;

using static KbdMod;
using static WCH;

/// <summary>Спеціальныя значенія wch[] изъ kbd.h.</summary>
public static class WCH
{
	public const char WCH_NONE = '\uF000';
	public const char WCH_DEAD = '\uF001';
	public const char WCH_LGTR = '\uF002';
}

/// <summary>Біты модификаторовъ изъ kbd.h.</summary>
[Flags]
public enum KbdMod
{
	None  = 0,
	Shift = 1,
	Ctrl  = 2,
	Kana  = 8,
}

/// <summary>Атрибуты строки VK_TO_WCHARS изъ kbd.h.</summary>
[Flags]
public enum Attr
{
	None   = 0,
	CapLok = 1,
	SgCaps = 2,
}

/// <summary>Одна запись VK_TO_WCHARS: основная строка и (если есть) слѣдующая за ней строка VK__none_.</summary>
public sealed class VK_TO_WCHARS
{
	public Attr Attr;
	public char[] Main = [];
	public char[] Extra = [];
}

/// <summary>Разкладка, прочитанная изъ .cpp или .dll.</summary>
public sealed class KBDTABLES
{
	/// <summary>Языкъ раскладки (младшіе 16 бітъ KLID): съ какой сѵстемной разкраской сравнивать.</summary>
	public ushort LCID = 0x0409;

	/// <summary>Значеніе ModNumber для «модификаторъ недопустимъ» (SHFT_INVALID).</summary>
	public const int SHFT_INVALID = 0x0F;

	/// <summary>Индексъ — битовая маска модификаторовъ, значеніе — номеръ столбца wch[].</summary>
	public int[] ModNumber = [0, 1, 2, SHFT_INVALID, SHFT_INVALID, SHFT_INVALID, SHFT_INVALID, SHFT_INVALID, 3, 4];

	public int[] ScanToVk = [.. Repeat(-1, 0x80)];

	public Dictionary<ushort, int> E0ToVk = [];

	public Dictionary<int, VK_TO_WCHARS> Rows = [];

	public int Vk(ushort scan, bool ext)
	{
		if (ext) return E0ToVk.TryGetValue(scan, out int vk) ? vk : -1;
		return scan < ScanToVk.Length ? ScanToVk[scan] : -1;
	}

	static char Pick(char[] a, int mod) => 0 <= mod && mod < a.Length ? a[mod] : WCH_NONE;

	int Mod(KbdMod bits) => (int)bits < ModNumber.Length && ModNumber[(int)bits] != SHFT_INVALID ? ModNumber[(int)bits] : -1;

	/// <summary>Символъ основной строки для маски модификаторовъ. Ноль допустимъ только на Ctrl (Ctrl+@ = NUL).</summary>
	public char GetMain(VK_TO_WCHARS row, KbdMod bits)
	{
		char v = Pick(row.Main, Mod(bits));
		return v == 0 && bits != Ctrl ? WCH_NONE : v;
	}

	/// <summary>Символъ дополнительной строки (Caps или знакъ мёртвой клавиши); 0 — «нѣтъ».</summary>
	public char GetExtra(VK_TO_WCHARS row, KbdMod bits)
	{
		char v = Pick(row.Extra, Mod(bits));
		return v == 0 ? WCH_NONE : v;
	}
}
