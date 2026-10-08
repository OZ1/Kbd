namespace KbdImage.Model;

using static KeyKind;

public enum KeyKind { Data, Label, Tab, BackSpace, Shift, Enter, Win, Menu, }

/// <summary>Положеніе кнопки въ логическихъ единицахъ (пиксели образца).</summary>
public sealed record KeyDef(KeyKind Kind, float X, float Y, float W, float H, ushort Scan = 0, bool Ext = false, string Label = "");

/// <summary>Расположеніе кнопокъ: основной блокъ 15 u и цифровой блокъ, придвинутый къ нему.</summary>
public static class Geometry
{
	public const float U = 85f;
	public const float RowH = 83f;
	public const float NumGap = 0.25f * U;

	public static readonly List<KeyDef> Keys = Build();

	public static readonly float Width = 15 * U + NumGap + 4 * U;
	public static readonly float Height = 5 * RowH;

	static List<KeyDef> Build()
	{
		List<KeyDef> k = [];

		void Row(int row, float x, params (KeyKind Kind, float W, ushort Scan, bool Ext, string Label)[] keys)
		{
			foreach ((KeyKind kind, float w, ushort scan, bool ext, string label) in keys)
			{
				k.Add(new KeyDef(kind, x * U, row * RowH, w * U, RowH, scan, ext, label));
				x += w;
			}
		}

		(KeyKind, float, ushort, bool, string) D(ushort scan, float w = 1, bool ext = false) => (Data, w, scan, ext, "");
		(KeyKind, float, ushort, bool, string) L(string label, float w) => (Label, w, 0, false, label);
		(KeyKind, float, ushort, bool, string) S(KeyKind kind, float w) => (kind, w, 0, false, "");

		List<(KeyKind, float, ushort, bool, string)> Scans(ushort first, ushort last)
		{
			List<(KeyKind, float, ushort, bool, string)> l = [];
			for (ushort s = first; s <= last; s++)
				l.Add(D(s));
			return l;
		}

		Row(0, 0, [D(0x29), .. Scans(0x02, 0x0D), S(BackSpace, 2)]);
		Row(1, 0, [S(Tab, 1.5f), .. Scans(0x10, 0x1B), D(0x2B, 1.5f)]);
		Row(2, 0, [L("Caps Lock", 1.75f), .. Scans(0x1E, 0x28), (Enter, 2.25f, 0x1C, false, "")]);
		Row(3, 0, [S(Shift, 2.25f), .. Scans(0x2C, 0x35), S(Shift, 2.75f)]);
		Row(4, 0,  L("Ctrl", 1.125f), S(Win, 1.125f), L("Alt", 1.25f), D(0x39, 6.25f),
		/**/       L("Kana", 1.25f),  S(Win, 1.25f),  S( Menu, 1.25f), L("Ctrl", 1.5f));

		float nx = 15 + NumGap / U;
		Row(0, nx, L("Num\nLock", 1), D(0x35, 1, true), D(0x37), D(0x4A));
		Row(1, nx, D(0x47), D(0x48), D(0x49));
		Row(2, nx, D(0x4B), D(0x4C), D(0x4D));
		Row(3, nx, D(0x4F), D(0x50), D(0x51));
		Row(4, nx, D(0x52, 2), D(0x53));
		k.Add(new(Data, (nx + 3) * U, 1 * RowH, U, 2 * RowH, 0x4E));
		k.Add(new(Data, (nx + 3) * U, 3 * RowH, U, 2 * RowH, 0x1C, true));
		return k;
	}
}
