using Microsoft.Win32;

namespace KbdImage.Parsing;

using Model;

using static Path;
using static Environment;

/// <summary>Имена константъ изъ kbd.h / winuser.h и таблица скан-код → VK изъ установленной разкладки (KbdUS.dll, KbdRU.dll, …).</summary>
public static class VkDefaults
{
	const string LayoutsKey = @"SYSTEM\CurrentControlSet\Control\Keyboard Layouts\";
	const string DefaultFile = "KBDUS.DLL";

	static readonly Dictionary<string, int> Consts = new()
	{
		{ "WCH_NONE", WCH.WCH_NONE }, { "WCH_DEAD", WCH.WCH_DEAD }, { "WCH_LGTR", WCH.WCH_LGTR },
		{ "CAPLOK", 1 }, { "SGCAPS", 2 }, { "CAPLOKALTGR", 4 }, { "KANALOK", 8 }, { "GRPSELTAP", 0x80 },
		{ "VK__none_", 0xFF },
		{ "VK_CANCEL", 0x03 },
		{ "VK_SPACE", ' ' }, { "VK_BACK", '\b' }, { "VK_TAB", '\t' }, { "VK_RETURN", '\r' }, { "VK_ESCAPE", '\e' },
		{ "VK_SHIFT", 0x10 }, { "VK_CONTROL", 0x11 }, { "VK_MENU", 0x12 }, { "VK_CAPITAL", 0x14 },
		{ "VK_NUMPAD0", 0x60 }, { "VK_NUMPAD1", 0x61 }, { "VK_NUMPAD2", 0x62 }, { "VK_NUMPAD3", 0x63 }, { "VK_NUMPAD4", 0x64 },
		{ "VK_NUMPAD5", 0x65 }, { "VK_NUMPAD6", 0x66 }, { "VK_NUMPAD7", 0x67 }, { "VK_NUMPAD8", 0x68 }, { "VK_NUMPAD9", 0x69 },
		{ "VK_MULTIPLY", 0x6A }, { "VK_ADD"    , 0x6B }, { "VK_SEPARATOR", 0x6C },
		{ "VK_SUBTRACT", 0x6D }, { "VK_DECIMAL", 0x6E }, { "VK_DIVIDE"   , 0x6F },
		{ "VK_NUMLOCK", 0x90 },
		{ "VK_OEM_1", 0xBA }, { "VK_OEM_PLUS", 0xBB }, { "VK_OEM_COMMA", 0xBC }, { "VK_OEM_MINUS", 0xBD }, { "VK_OEM_PERIOD", 0xBE },
		{ "VK_OEM_2", 0xBF }, { "VK_OEM_3", 0xC0 }, { "VK_OEM_4", 0xDB }, { "VK_OEM_5", 0xDC },
		{ "VK_OEM_6", 0xDD }, { "VK_OEM_7", 0xDE }, { "VK_OEM_8", 0xDF }, { "VK_OEM_102", 0xE2 },
		{ "VK_ABNT_C1", 0xC1 }, { "VK_ABNT_C2", 0xC2 },
	};

	static readonly Dictionary<ushort, KBDTABLES> Cache = [];

	/// <summary>Константа по имени; -1 — неизвѣстна.</summary>
	public static int Const(string name) => Consts.TryGetValue(name, out int v) ? v : -1;

	/// <summary>Таблица скан-код → VK изъ сѵстемной разкладки языка lcid (KLID 0000xxxx); нѣтъ такой — изъ KbdUS.dll.</summary>
	public static void Fill(KBDTABLES l, ushort lcid)
	{
		KBDTABLES s = Installed(lcid);
		s.ScanToVk.CopyTo(l.ScanToVk, 0);
		foreach (KeyValuePair<ushort, int> e in s.E0ToVk)
			l.E0ToVk[e.Key] = e.Value;
	}

	/// <summary>Установленная сѵстемная разкладка языка lcid (прочитанная .dll, съ кэшемъ).</summary>
	public static KBDTABLES Installed(ushort lcid)
	{
		if (!Cache.TryGetValue(lcid, out KBDTABLES? s))
			Cache.Add(lcid, s = PeLayoutReader.Read(FindFile(lcid)));
		return s;
	}

	/// <summary>Путь къ .dll изъ «Keyboard Layouts\0000xxxx\Layout File»; при неудачѣ — KbdUS.dll.</summary>
	static string FindFile(ushort lcid)
	{
		using RegistryKey? k = Registry.LocalMachine.OpenSubKey($"{LayoutsKey}{lcid:X8}");
		if (k?.GetValue("Layout File") is string { Length: > 0 } file && File.Exists(Combine(SystemDirectory, file)))
			return Combine(SystemDirectory, file);
		return Combine(SystemDirectory, DefaultFile);
	}
}
