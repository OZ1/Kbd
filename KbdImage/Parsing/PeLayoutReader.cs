using System.Text;

namespace KbdImage.Parsing;

using Model;

using static File;
using static Math;
using static Encoding;
using static BitConverter;

using static Model.WCH;

/// <summary>Читаетъ таблицы разкладки изъ собранной kbd-.dll (x86, x64, ARM64) безъ загрузки ея въ процессъ.</summary>
public sealed class PeLayoutReader(byte[] PE)
{
	const int KbdNumPad = 0x0800;

	readonly List<(uint Va, uint Size, uint Raw, uint RawSize)> Sections = [];
	ulong ImageBase;
	int _ps;

	public static KBDTABLES Read(string path) => new PeLayoutReader(ReadAllBytes(path)).ReadLayout();

	KBDTABLES ReadLayout()
	{
		if (PE.Length < 0x100 || PE[0] != 'M' || PE[1] != 'Z')
			throw new InvalidDataException("Не PE-файлъ");
		int pe = ToInt32(PE, 0x3C);
		if (ToUInt32(PE, pe) != 0x00004550)
			throw new InvalidDataException("Нѣтъ сигнатуры PE");
		ushort machine = ToUInt16(PE, pe + 4);
		int nSec = ToUInt16(PE, pe + 6);
		int optSize = ToUInt16(PE, pe + 20);
		int opt = pe + 24;
		ushort magic = ToUInt16(PE, opt);
		bool plus = magic == 0x20B;
		_ps = plus ? 8 : 4;
		ImageBase = plus ? ToUInt64(PE, opt + 24) : ToUInt32(PE, opt + 28);
		uint exportRva = ToUInt32(PE, opt + (plus ? 112 : 96));
		for (int i = 0; i < nSec; i++)
		{
			int s = opt + optSize + i * 40;
			Sections.Add((ToUInt32(PE, s + 12), ToUInt32(PE, s + 8), ToUInt32(PE, s + 20), ToUInt32(PE, s + 16)));
		}

		uint funcRva = FindExport(exportRva, "KbdLayerDescriptor");
		uint tablesRva = machine switch
		{
			0xAA64 => DecodeArm64(funcRva),
			0x8664 => DecodeX64  (funcRva),
			_      => DecodeX86  (funcRva),
		};
		return ReadTables(tablesRva);
	}

	int Off(uint rva)
	{
		foreach ((uint va, uint size, uint raw, uint rawSize) in Sections)
			if (rva >= va && rva < va + Max(size, rawSize))
				return (int)(rva - va + raw);
		throw new InvalidDataException($"RVA 0x{rva:X} внѣ секцій");
	}

	uint FindExport(uint dirRva, string name)
	{
		if (dirRva == 0)
			throw new InvalidDataException("Нѣтъ таблицы экспорта");
		int d = Off(dirRva);
		int nNames =     ToInt32(PE, d + 24);
		int funcs = Off(ToUInt32(PE, d + 28));
		int names = Off(ToUInt32(PE, d + 32));
		int ords  = Off(ToUInt32(PE, d + 36));
		for (int i = 0; i < nNames; i++)
		{
			int n = Off(ToUInt32(PE, names + 4 * i));
			int e = n;
			while (PE[e] != 0) e++;
			if (ASCII.GetString(PE, n, e - n) == name)
				return ToUInt32(PE, funcs + 4 * ToUInt16(PE, ords + 2 * i));
		}
		throw new InvalidDataException("Нѣтъ экспорта KbdLayerDescriptor");
	}

	// lea rax, [rip+rel32]; ret
	uint DecodeX64(uint f)
	{
		int o = Off(f);
		for (int i = 0; i < 16; i++)
			if (PE[o + i] == 0x48 && PE[o + i + 1] == 0x8D && PE[o + i + 2] == 0x05)
				return (uint)(f + i + 7 + ToInt32(PE, o + i + 3));
		throw new InvalidDataException("Не распознана KbdLayerDescriptor (x64)");
	}

	// mov eax, imm32; ret
	uint DecodeX86(uint f)
	{
		int o = Off(f);
		for (int i = 0; i < 16; i++)
			if (PE[o + i] == 0xB8)
				return (uint)(ToUInt32(PE, o + i + 1) - ImageBase);
		throw new InvalidDataException("Не распознана KbdLayerDescriptor (x86)");
	}

	// adrp x0, page; add x0, x0, #off; ret
	uint DecodeArm64(uint f)
	{
		int o = Off(f);
		for (int i = 0; i < 6; i++)
		{
			uint ins = ToUInt32(PE, o + 4 * i);
			if ((ins & 0x9F00001F) != 0x90000000)
				continue;
			long imm = (((long)((ins >> 5) & 0x7FFFF) << 2) | ((ins >> 29) & 3)) << 12;
			if ((imm & (1L << 32)) != 0)
				imm -= 1L << 33;
			long page = ((f + 4L * i) & ~0xFFFL) + imm;
			uint add = ToUInt32(PE, o + 4 * (i + 1));
			if ((add & 0xFF80001F) != 0x91000000)
				throw new InvalidDataException("Не распознана KbdLayerDescriptor (ARM64)");
			long off12 = (add >> 10) & 0xFFF;
			if ((add & (1u << 22)) != 0)
				off12 <<= 12;
			return (uint)(page + off12);
		}
		throw new InvalidDataException("Не распознана KbdLayerDescriptor (ARM64)");
	}

	uint Ptr(int off)
	{
		ulong va = _ps == 8 ? ToUInt64(PE, off) : ToUInt32(PE, off);
		return va == 0 ? 0 : (uint)(va - ImageBase);
	}

	KBDTABLES ReadTables(uint tablesRva)
	{
		KBDTABLES l = new();
		int t = Off(tablesRva);
		uint mods = Ptr(t);
		if (mods != 0)
		{
			int m = Off(mods);
			int count = ToUInt16(PE, m + _ps) + 1;
			int[] mn = new int[count];
			for (int i = 0; i < count; i++)
				mn[i] = PE[m + _ps + 2 + i];
			l.ModNumber = mn;
		}

		// scancode -> VK
		uint vscRva = Ptr(t + 6 * _ps);
		int maxVsc = PE[t + 7 * _ps];
		if (vscRva != 0)
		{
			int v = Off(vscRva);
			for (int s = 0; s < Min(maxVsc, l.ScanToVk.Length); s++)
				l.ScanToVk[s] = Normalize(ToUInt16(PE, v + 2 * s));
		}
		uint e0 = Ptr(t + 8 * _ps);
		if (e0 != 0)
			for (int p = Off(e0); PE[p] != 0 || ToUInt16(PE, p + 2) != 0; p += 4)
				l.E0ToVk[PE[p]] = ToUInt16(PE, p + 2) & 0xFF;

		// VK_TO_WCHAR_TABLE
		uint wt = Ptr(t + _ps);
		if (wt == 0) return l;
		for (int p = Off(wt); ; p += 2 * _ps)
		{
			uint rows = Ptr(p);
			if (rows == 0)
				break;
			int nMod = PE[p + _ps];
			int cb = PE[p + _ps + 1];
			VK_TO_WCHARS? last = null;
			for (int r = Off(rows); PE[r] != 0; r += cb)
			{
				char[] wch = new char[nMod];
				for (int c = 0; c < nMod; c++)
					wch[c] = (char)ToUInt16(PE, r + 2 + 2 * c);
				int vk = PE[r];
				// Въ .dll нѣтъ отличія «не задано» отъ '\0': NUL оставляемъ только за Ctrl+@ (клавиша «2»)
				if (vk != 0xFF)
					for (int c = 0; c < nMod; c++)
						if (wch[c] == 0 && !(vk == 0x32 && c == l.ModNumber[(int)KbdMod.Ctrl]))
							wch[c] = WCH_NONE;
				if (vk == 0xFF && last != null)
					last.Extra = wch;
				else
				{
					last = new VK_TO_WCHARS { Attr = (Attr)PE[r + 1], Main = wch };
					l.Rows.TryAdd(vk, last);
				}
			}
		}
		return l;
	}

	/// <summary>VK изъ записи ausVK: сбрасываетъ флаги; для цифровыхъ (KBDNUMPAD) переводитъ Home/Up/… въ VK_NUMPADn.</summary>
	static int Normalize(int entry)
	{
		int vk = entry & 0xFF;
		if ((entry & KbdNumPad) == 0)
			return vk == 0 ? -1 : vk;
		return vk switch
		{
			0x24 => 0x67, 0x26 => 0x68, 0x21 => 0x69,
			0x25 => 0x64, 0x0C => 0x65, 0x27 => 0x66,
			0x23 => 0x61, 0x28 => 0x62, 0x22 => 0x63,
			0x2D => 0x60, 0x2E => 0x6E,
			_ => vk,
		};
	}
}
