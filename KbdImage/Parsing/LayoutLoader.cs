using System.Globalization;
using System.Text.RegularExpressions;
using static System.IO.File;

namespace KbdImage.Parsing;

using Model;

using static Path;
using static UInt16;
using static NumberStyles;
using static StringComparison;

public static partial class LayoutLoader
{
	/// <summary>Загружаетъ разкладку. Языкъ (для сравненія съ сѵстемной разкраской) — изъ klid, а безъ него изъ Setup\Kbd.wxs.</summary>
	public static KBDTABLES Load(string path, string? klid = null)
	{
		klid ??= FindKlid(path);
		ushort lcid = klid is { Length: >= 4 } && TryParse(klid[^4..], HexNumber, CultureInfo.InvariantCulture, out ushort l) ? l : (ushort)0x0409;
		if (!".dll".Equals(GetExtension(path), OrdinalIgnoreCase))
			return CppLayoutReader.Read(path, lcid);
		KBDTABLES layout = PeLayoutReader.Read(path);
		layout.LCID = lcid;
		return layout;
	}

	/// <summary>Ищетъ KLID разкладки въ Setup\Kbd.wxs, поднимаясь по папкамъ отъ файла (MyKbdSlav.cpp, KbdSlav.dll → «KbdSlav»).</summary>
	static string? FindKlid(string path)
	{
		string name = GetFileNameWithoutExtension(path);
		if (name.StartsWith("My", OrdinalIgnoreCase))
			name = name[2..];
		for (string? dir = GetDirectoryName(GetFullPath(path)); dir != null; dir = GetDirectoryName(dir))
		{
			string wxs = Combine(dir, "Setup", "Kbd.wxs");
			if (!Exists(wxs))
				continue;
			foreach (Match m in KlidRegex().Matches(ReadAllText(wxs)))
				if (m.Groups["n"].Value.Equals(name, OrdinalIgnoreCase))
					return m.Groups["k"].Value;
			return null;
		}
		return null;
	}

	[GeneratedRegex(@"\$\(Layout\)\s*=\s*""(?<n>\w+)""\s*\?>\s*<\?define\s+Klid\s*=\s*""(?<k>[0-9a-fA-F]{8})""")]
	private static partial Regex KlidRegex();
}
