namespace KbdImage.Model;

/// <summary>Сѵстемная разкраска, съ которой сравнивается разкладка: (обычная, Shift) по скан-коду. 0 — символа нѣтъ.</summary>
public sealed class SystemLayout
{
	static readonly Dictionary<ushort, SystemLayout> Cache = [];

	readonly Dictionary<ushort, (char Normal, char Shift)> Map = [];

	SystemLayout(ushort lcid)
	{
		void Run(ushort firstScan, string normal, string shift)
		{
			for (int i = 0; i < normal.Length; i++)
				Map[(ushort)(firstScan + i)] = (normal[i], shift[i]);
		}

		if (lcid == 0x0419)
		{
			// ЙЦУКЕН
			Run(0x02, "1234567890-=", "!\"№;%:?*()_+");
			Run(0x10, "йцукенгшщзхъ", "ЙЦУКЕНГШЩЗХЪ");
			Run(0x1E, "фывапролджэ", "ФЫВАПРОЛДЖЭ");
			Run(0x2C, "ячсмитьбю.", "ЯЧСМИТЬБЮ,");
			Map[0x29] = ('ё', 'Ё');
			Map[0x2B] = ('\\', '/');
			Map[0x56] = ('\\', '/');
		}
		else
		{
			// US QWERTY
			Run(0x02, "1234567890-=", "!@#$%^&*()_+");
			Run(0x10, "qwertyuiop[]", "QWERTYUIOP{}");
			Run(0x1E, "asdfghjkl;'", "ASDFGHJKL:\"");
			Run(0x2C, "zxcvbnm,./", "ZXCVBNM<>?");
			Map[0x29] = ('`', '~');
			Map[0x2B] = ('\\', '|');
			Map[0x56] = ('\\', '|');
		}

		Map[0x1C] = ('\r', '\r');
		Map[0x39] = (' ', ' ');
		Map[0x37] = ('*', '*');
		Map[0x4A] = ('-', '-');
		Map[0x4E] = ('+', '+');
		Map[0x53] = ('.', '\0');
		Map[0x135] = ('/', '/');
		Map[0x11C] = ('\r', '\r');
		ushort[] pad = [0x47, 0x48, 0x49, 0x4B, 0x4C, 0x4D, 0x4F, 0x50, 0x51, 0x52];
		char[] digits = ['7', '8', '9', '4', '5', '6', '1', '2', '3', '0'];
		for (int i = 0; i < pad.Length; i++)
			Map[pad[i]] = (digits[i], '\0');
	}

	/// <summary>Разкраска для языка раскладки (младшіе 16 бітъ KLID: 0x0419 — русская, иначе US).</summary>
	public static SystemLayout For(ushort lcid)
	{
		ushort key = lcid == 0x0419 ? lcid : (ushort)0x0409;
		if (!Cache.TryGetValue(key, out SystemLayout? s))
			Cache.Add(key, s = new(key));
		return s;
	}

	public bool TryGet(ushort scan, bool ext, out (char Normal, char Shift) chars)
		=> Map.TryGetValue((ushort)(ext ? scan | 0x100 : scan), out chars);
}
