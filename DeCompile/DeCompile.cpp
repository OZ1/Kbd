#include <Windows.h>
#include <kbd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <io.h>
#include <algorithm>

#define VK_POWER 0x5E

static const BYTE ausVK[] = {
	T00, T01, T02, T03, T04, T05, T06, T07, T08, T09, T0A, T0B, T0C, T0D, T0E, T0F,
	T10, T11, T12, T13, T14, T15, T16, T17, T18, T19, T1A, T1B, T1C, T1D, T1E, T1F,
	T20, T21, T22, T23, T24, T25, T26, T27, T28, T29, T2A, T2B, T2C, T2D, T2E, T2F,
	T30, T31, T32, T33, T34, T35, T36, T37, T38, T39, T3A, T3B, T3C, T3D, T3E, T3F,
	T40, T41, T42, T43, T44, T45, T46, T47, T48, T49, T4A, T4B, T4C, T4D, T4E, T4F,
	T50, T51, T52, T53, T54, T55, T56, T57, T58, T59, T5A, T5B, T5C, T5D, T5E, T5F,
	T60, T61, T62, T63, T64, T65, T66, T67, T68, T69, T6A, T6B, T6C, T6D, T6E, T6F,
	T70, T71, T72, T73, T74, T75, T76, T77, T78, T79, T7A, T7B, T7C, T7D, T7E, T7F,
};
static_assert(sizeof ausVK == 0x80);

static const USHORT ausVK_E0[] = {
	X10 | 0x10 << 8,
	X19 | 0x19 << 8,
	X1C | 0x1C << 8,
	X1D | 0x1D << 8,
	X20 | 0x20 << 8,
	X21 | 0x21 << 8,
	X22 | 0x22 << 8,
	X24 | 0x24 << 8,
	X2E | 0x2E << 8,
	X30 | 0x30 << 8,
	X32 | 0x32 << 8,
	X35 | 0x35 << 8,
	X37 | 0x37 << 8,
	X38 | 0x38 << 8,
	X46 | 0x46 << 8,
	X47 | 0x47 << 8,
	X48 | 0x48 << 8,
	X49 | 0x49 << 8,
	X4B | 0x4B << 8,
	X4D | 0x4D << 8,
	X4F | 0x4F << 8,
	X50 | 0x50 << 8,
	X51 | 0x51 << 8,
	X52 | 0x52 << 8,
	X53 | 0x53 << 8,
	X5B | 0x5B << 8,
	X5C | 0x5C << 8,
	X5D | 0x5D << 8,
	X5E | 0x5E << 8,
	X5F | 0x5F << 8,
	X65 | 0x65 << 8,
	X66 | 0x66 << 8,
	X67 | 0x67 << 8,
	X68 | 0x68 << 8,
	X69 | 0x69 << 8,
	X6A | 0x6A << 8,
	X6B | 0x6B << 8,
	X6C | 0x6C << 8,
	X6D | 0x6D << 8,
};

static const wchar_t* const VK[] = {
	L"0",
	L"VK_LBUTTON"                        , // 01
	L"VK_RBUTTON"                        , // 02
	L"VK_CANCEL"                         , // 03
	L"VK_MBUTTON"                        , // 04
	L"VK_XBUTTON1"                       , // 05
	L"VK_XBUTTON2"                       , // 06
	L"0x07",
	L"VK_BACK"                           , // 08
	L"VK_TAB"                            , // 09
	L"0x0A", L"0x0B",
	L"VK_CLEAR"                          , // 0C
	L"VK_RETURN"                         , // 0D
	L"0x0E", L"0x0F",
	L"VK_SHIFT"                          , // 10
	L"VK_CONTROL"                        , // 11
	L"VK_MENU"                           , // 12
	L"VK_PAUSE"                          , // 13
	L"VK_CAPITAL"                        , // 14
	L"VK_KANA"                           , // 15
	L"VK_IME_ON"                         , // 16
	L"VK_JUNJA"                          , // 17
	L"VK_FINAL"                          , // 18
	L"VK_KANJI"                          , // 19
	L"VK_IME_OFF"                        , // 1A
	L"VK_ESCAPE"                         , // 1B
	L"VK_CONVERT"                        , // 1C
	L"VK_NONCONVERT"                     , // 1D
	L"VK_ACCEPT"                         , // 1E
	L"VK_MODECHANGE"                     , // 1F
	L"VK_SPACE"                          , // 20
	L"VK_PRIOR"                          , // 21
	L"VK_NEXT"                           , // 22
	L"VK_END"                            , // 23
	L"VK_HOME"                           , // 24
	L"VK_LEFT"                           , // 25
	L"VK_UP"                             , // 26
	L"VK_RIGHT"                          , // 27
	L"VK_DOWN"                           , // 28
	L"VK_SELECT"                         , // 29
	L"VK_PRINT"                          , // 2A
	L"VK_EXECUTE"                        , // 2B
	L"VK_SNAPSHOT"                       , // 2C
	L"VK_INSERT"                         , // 2D
	L"VK_DELETE"                         , // 2E
	L"VK_HELP"                           , // 2F
	L"'0'", L"'1'", L"'2'", L"'3'", L"'4'", L"'5'", L"'6'", L"'7'", L"'8'", L"'9'", L"0x3A", L"0x3B", L"0x3C", L"0x3D", L"0x3E", L"0x3F", L"0x40",
	L"'A'", L"'B'", L"'C'", L"'D'", L"'E'", L"'F'", L"'G'", L"'H'", L"'I'", L"'J'", L"'K'", L"'L'", L"'M'", L"'N'", L"'O'", L"'P'", L"'Q'", L"'R'", L"'S'", L"'T'", L"'U'", L"'V'", L"'W'", L"'X'", L"'Y'", L"'Z'",
	L"VK_LWIN"                           , // 5B
	L"VK_RWIN"                           , // 5C
	L"VK_APPS"                           , // 5D
	L"VK_POWER"                          , // 5E
	L"VK_SLEEP"                          , // 5F
	L"VK_NUMPAD0"                        , // 60
	L"VK_NUMPAD1"                        , // 61
	L"VK_NUMPAD2"                        , // 62
	L"VK_NUMPAD3"                        , // 63
	L"VK_NUMPAD4"                        , // 64
	L"VK_NUMPAD5"                        , // 65
	L"VK_NUMPAD6"                        , // 66
	L"VK_NUMPAD7"                        , // 67
	L"VK_NUMPAD8"                        , // 68
	L"VK_NUMPAD9"                        , // 69
	L"VK_MULTIPLY"                       , // 6A
	L"VK_ADD"                            , // 6B
	L"VK_SEPARATOR"                      , // 6C
	L"VK_SUBTRACT"                       , // 6D
	L"VK_DECIMAL"                        , // 6E
	L"VK_DIVIDE"                         , // 6F
	L"VK_F1"                             , // 70
	L"VK_F2"                             , // 71
	L"VK_F3"                             , // 72
	L"VK_F4"                             , // 73
	L"VK_F5"                             , // 74
	L"VK_F6"                             , // 75
	L"VK_F7"                             , // 76
	L"VK_F8"                             , // 77
	L"VK_F9"                             , // 78
	L"VK_F10"                            , // 79
	L"VK_F11"                            , // 7A
	L"VK_F12"                            , // 7B
	L"VK_F13"                            , // 7C
	L"VK_F14"                            , // 7D
	L"VK_F15"                            , // 7E
	L"VK_F16"                            , // 7F
	L"VK_F17"                            , // 80
	L"VK_F18"                            , // 81
	L"VK_F19"                            , // 82
	L"VK_F20"                            , // 83
	L"VK_F21"                            , // 84
	L"VK_F22"                            , // 85
	L"VK_F23"                            , // 86
	L"VK_F24"                            , // 87
	L"VK_NAVIGATION_VIEW"                , // 88
	L"VK_NAVIGATION_MENU"                , // 89
	L"VK_NAVIGATION_UP"                  , // 8A
	L"VK_NAVIGATION_DOWN"                , // 8B
	L"VK_NAVIGATION_LEFT"                , // 8C
	L"VK_NAVIGATION_RIGHT"               , // 8D
	L"VK_NAVIGATION_ACCEPT"              , // 8E
	L"VK_NAVIGATION_CANCEL"              , // 8F
	L"VK_NUMLOCK"                        , // 90
	L"VK_SCROLL"                         , // 91
	L"VK_OEM_NEC_EQUAL"                  , // 92   // '=' key on numpad
	L"VK_OEM_FJ_MASSHOU"                 , // 93   // 'Unregister word' key
	L"VK_OEM_FJ_TOUROKU"                 , // 94   // 'Register word' key
	L"VK_OEM_FJ_LOYA"                    , // 95   // 'Left  OYAYUBI' key
	L"VK_OEM_FJ_ROYA"                    , // 96   // 'Right OYAYUBI' key
	L"0x97", L"0x98", L"0x99", L"0x9A", L"0x9B", L"0x9C", L"0x9D", L"0x9E", L"0x9F",
	L"VK_LSHIFT"                         , // A0
	L"VK_RSHIFT"                         , // A1
	L"VK_LCONTROL"                       , // A2
	L"VK_RCONTROL"                       , // A3
	L"VK_LMENU"                          , // A4
	L"VK_RMENU"                          , // A5
	L"VK_BROWSER_BACK"                   , // A6
	L"VK_BROWSER_FORWARD"                , // A7
	L"VK_BROWSER_REFRESH"                , // A8
	L"VK_BROWSER_STOP"                   , // A9
	L"VK_BROWSER_SEARCH"                 , // AA
	L"VK_BROWSER_FAVORITES"              , // AB
	L"VK_BROWSER_HOME"                   , // AC
	L"VK_VOLUME_MUTE"                    , // AD
	L"VK_VOLUME_DOWN"                    , // AE
	L"VK_VOLUME_UP"                      , // AF
	L"VK_MEDIA_NEXT_TRACK"               , // B0
	L"VK_MEDIA_PREV_TRACK"               , // B1
	L"VK_MEDIA_STOP"                     , // B2
	L"VK_MEDIA_PLAY_PAUSE"               , // B3
	L"VK_LAUNCH_MAIL"                    , // B4
	L"VK_LAUNCH_MEDIA_SELECT"            , // B5
	L"VK_LAUNCH_APP1"                    , // B6
	L"VK_LAUNCH_APP2"                    , // B7
	L"0xB8", L"0xB9",
	L"VK_OEM_1"                          , // BA   // ';:' for US
	L"VK_OEM_PLUS"                       , // BB   // '+' any country
	L"VK_OEM_COMMA"                      , // BC   // ',' any country
	L"VK_OEM_MINUS"                      , // BD   // '-' any country
	L"VK_OEM_PERIOD"                     , // BE   // '.' any country
	L"VK_OEM_2"                          , // BF   // '/?' for US
	L"VK_OEM_3"                          , // C0   // '`~' for US
	L"VK_ABNT_C1",
	L"VK_ABNT_C2",
	L"VK_GAMEPAD_A"                      , // C3
	L"VK_GAMEPAD_B"                      , // C4
	L"VK_GAMEPAD_X"                      , // C5
	L"VK_GAMEPAD_Y"                      , // C6
	L"VK_GAMEPAD_RIGHT_SHOULDER"         , // C7
	L"VK_GAMEPAD_LEFT_SHOULDER"          , // C8
	L"VK_GAMEPAD_LEFT_TRIGGER"           , // C9
	L"VK_GAMEPAD_RIGHT_TRIGGER"          , // CA
	L"VK_GAMEPAD_DPAD_UP"                , // CB
	L"VK_GAMEPAD_DPAD_DOWN"              , // CC
	L"VK_GAMEPAD_DPAD_LEFT"              , // CD
	L"VK_GAMEPAD_DPAD_RIGHT"             , // CE
	L"VK_GAMEPAD_MENU"                   , // CF
	L"VK_GAMEPAD_VIEW"                   , // D0
	L"VK_GAMEPAD_LEFT_THUMBSTICK_BUTTON" , // D1
	L"VK_GAMEPAD_RIGHT_THUMBSTICK_BUTTON", // D2
	L"VK_GAMEPAD_LEFT_THUMBSTICK_UP"     , // D3
	L"VK_GAMEPAD_LEFT_THUMBSTICK_DOWN"   , // D4
	L"VK_GAMEPAD_LEFT_THUMBSTICK_RIGHT"  , // D5
	L"VK_GAMEPAD_LEFT_THUMBSTICK_LEFT"   , // D6
	L"VK_GAMEPAD_RIGHT_THUMBSTICK_UP"    , // D7
	L"VK_GAMEPAD_RIGHT_THUMBSTICK_DOWN"  , // D8
	L"VK_GAMEPAD_RIGHT_THUMBSTICK_RIGHT" , // D9
	L"VK_GAMEPAD_RIGHT_THUMBSTICK_LEFT"  , // DA
	L"VK_OEM_4"                          , // DB  //  '[{' for US
	L"VK_OEM_5"                          , // DC  //  '\|' for US
	L"VK_OEM_6"                          , // DD  //  ']}' for US
	L"VK_OEM_7"                          , // DE  //  ''"' for US
	L"VK_OEM_8"                          , // DF
	L"0xE0",
	L"VK_OEM_AX"                         , // E1  //  'AX' key on Japanese AX kbd
	L"VK_OEM_102"                        , // E2  //"  <>" or" \|" on RT 102-key kbd.
	L"VK_ICO_HELP"                       , // E3  //  Help key on ICO
	L"VK_ICO_00"                         , // E4  //  00 key on ICO
	L"VK_PROCESSKEY"                     , // E5
	L"VK_ICO_CLEAR"                      , // E6
	L"VK_PACKET"                         , // E7
	L"0xE8",
	L"VK_OEM_RESET"                      , // E9
	L"VK_OEM_JUMP"                       , // EA
	L"VK_OEM_PA1"                        , // EB
	L"VK_OEM_PA2"                        , // EC
	L"VK_OEM_PA3"                        , // ED
	L"VK_OEM_WSCTRL"                     , // EE
	L"VK_OEM_CUSEL"                      , // EF
	L"VK_OEM_ATTN"                       , // F0
	L"VK_OEM_FINISH"                     , // F1
	L"VK_OEM_COPY"                       , // F2
	L"VK_OEM_AUTO"                       , // F3
	L"VK_OEM_ENLW"                       , // F4
	L"VK_OEM_BACKTAB"                    , // F5
	L"VK_ATTN"                           , // F6
	L"VK_CRSEL"                          , // F7
	L"VK_EXSEL"                          , // F8
	L"VK_EREOF"                          , // F9
	L"VK_PLAY"                           , // FA
	L"VK_ZOOM"                           , // FB
	L"VK_NONAME"                         , // FC
	L"VK_PA1"                            , // FD
	L"VK_OEM_CLEAR"                      , // FE
	L"VK__none_"
};
static_assert(ARRAYSIZE(VK) == 0x100);

static void Print_VK_Flag(USHORT vkf)
{
	if (vkf & KBDEXT       ) wprintf_s(L" | KBDEXT");
	if (vkf & KBDMULTIVK   ) wprintf_s(L" | KBDMULTIVK");
	if (vkf & KBDNUMPAD    ) wprintf_s(L" | KBDNUMPAD");
	if (vkf & KBDSPECIAL   ) wprintf_s(L" | KBDSPECIAL");
	if (vkf & KBDUNICODE   ) wprintf_s(L" | KBDUNICODE");
	if (vkf & KBDINJECTEDVK) wprintf_s(L" | KBDINJECTEDVK");
	if (vkf & KBDMAPPEDVK  ) wprintf_s(L" | KBDMAPPEDVK");
	if (vkf & KBDBREAK     ) wprintf_s(L" | KBDBREAK");
}

static void Print_Modification(BYTE mod)
{
	switch (mod)
	{
	default:
	case 0b000: wprintf_s(L"          │"); break;
	case 0b001: wprintf_s(L"  Shift   │"); break;
	case 0b010: wprintf_s(L" Control  │"); break;
	case 0b100: wprintf_s(L"   Alt    │"); break;
	case 0b110: wprintf_s(L"  AltGr   │"); break;
	case 0b111: wprintf_s(L"S + AltGr │"); break;
	case 0b011: wprintf_s(L"Ctrl+Shift│"); break;
	}
}

static void Print_NLSFEProcIndex(BYTE NLSFEProcIndex)
{
	switch (NLSFEProcIndex)
	{
	case KBDNLS_NULL           : wprintf_s(L"KBDNLS_NULL           "); break;
	case KBDNLS_NOEVENT        : wprintf_s(L"KBDNLS_NOEVENT        "); break;
	case KBDNLS_SEND_BASE_VK   : wprintf_s(L"KBDNLS_SEND_BASE_VK   "); break;
	case KBDNLS_SEND_PARAM_VK  : wprintf_s(L"KBDNLS_SEND_PARAM_VK  "); break;
	case KBDNLS_KANALOCK       : wprintf_s(L"KBDNLS_KANALOCK       "); break;
	case KBDNLS_ALPHANUM       : wprintf_s(L"KBDNLS_ALPHANUM       "); break;
	case KBDNLS_HIRAGANA       : wprintf_s(L"KBDNLS_HIRAGANA       "); break;
	case KBDNLS_KATAKANA       : wprintf_s(L"KBDNLS_KATAKANA       "); break;
	case KBDNLS_SBCSDBCS       : wprintf_s(L"KBDNLS_SBCSDBCS       "); break;
	case KBDNLS_ROMAN          : wprintf_s(L"KBDNLS_ROMAN          "); break;
	case KBDNLS_CODEINPUT      : wprintf_s(L"KBDNLS_CODEINPUT      "); break;
	case KBDNLS_HELP_OR_END    : wprintf_s(L"KBDNLS_HELP_OR_END    "); break;
	case KBDNLS_HOME_OR_CLEAR  : wprintf_s(L"KBDNLS_HOME_OR_CLEAR  "); break;
	case KBDNLS_NUMPAD         : wprintf_s(L"KBDNLS_NUMPAD         "); break;
	case KBDNLS_KANAEVENT      : wprintf_s(L"KBDNLS_KANAEVENT      "); break;
	case KBDNLS_CONV_OR_NONCONV: wprintf_s(L"KBDNLS_CONV_OR_NONCONV"); break;
	default: wprintf_s(L"%hhu,", NLSFEProcIndex); break;
	}
}

int wmain(int argc, wchar_t* argv[])
{
	PKBDTABLES KbdTables;
	PKBDNLSTABLES KbdNlsTables{};
	if (_wcsicmp(argv[1] + wcslen(argv[1]) - 4, L".dll") == 0)
	{
		wchar_t* end;
		DWORD LoadFlag;
		if (argc == 2) LoadFlag = LOAD_LIBRARY_AS_IMAGE_RESOURCE;
		else if (argc != 3) return ERROR_INVALID_COMMAND_LINE;
		else if (LoadFlag = wcstoul(argv[2], &end, 16); *end) return ERROR_INVALID_COMMAND_LINE;

		auto const hLib = LoadLibraryEx(argv[1], nullptr, LoadFlag);
		if (!hLib) return GetLastError();

		if (auto const KbdLayerDescriptor = (PKBDTABLES(*)(void))GetProcAddress(hLib, (LPCSTR)1))
			KbdTables = KbdLayerDescriptor();
		else return GetLastError();

		if (auto const KbdNlsLayerDescriptor = (PKBDNLSTABLES(*)(void))GetProcAddress(hLib, (LPCSTR)2))
			KbdNlsTables = KbdNlsLayerDescriptor();
	}
	else
	{
		auto const hFile = CreateFile(argv[1], FILE_READ_DATA, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr, OPEN_EXISTING, 0, nullptr);
		if (hFile == INVALID_HANDLE_VALUE) return GetLastError();
		struct HANDLE_AUTO
		{
			HANDLE hFile;
			~HANDLE_AUTO(){CloseHandle(hFile);}
		} const cleanup{hFile};

		auto const size = GetFileSize(hFile, nullptr);
		KbdTables = (PKBDTABLES)VirtualAlloc((PVOID)(ULONG_PTR)0xF0000000, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
		if (!KbdTables) return GetLastError();

		if (!ReadFile(hFile, KbdTables, size, nullptr, nullptr)) return GetLastError();

		DWORD n, m;
		wchar_t* end;
		if (argc >= 3)
			if (n = wcstoul(argv[2], &end, 16); *end) return ERROR_INVALID_COMMAND_LINE;
			else (ULONG_PTR&)KbdTables += n;

		if (argc >= 4)
			if (m = wcstoul(argv[3], &end, 16); *end) return ERROR_INVALID_COMMAND_LINE;
			else KbdNlsTables = (PKBDNLSTABLES)((ULONG_PTR)KbdTables - n + m);
	}

	auto const unicode = _setmode(_fileno(stdout), _O_WTEXT) != -1;
	auto const cp1251 = !!SetConsoleOutputCP(1251);

	_putws(LR"(#define KBD_TYPE 4

#include <Windows.h>
#include <kbd.h>

#pragma data_seg(".data")
#define ALLOC_SECTION_LDATA __declspec(allocate(".data")))

#define VK_POWER 0x5E

#define VK_TO_WCHAR_TABLE_ENTRY(aVkToWchI) \
	{(PVK_TO_WCHARS1)aVkToWchI, ARRAYSIZE(aVkToWchI[0].wch), sizeof(aVkToWchI[0])}

/***************************************************************************\
* ausVK[] - Virtual Scan Code to Virtual Key conversion table
\***************************************************************************/
static ALLOC_SECTION_LDATA USHORT ausVK[] = {)");
	const BYTE bMaxVSCtoVK = min(ARRAYSIZE(ausVK), KbdTables->bMaxVSCtoVK);
	for (auto vsc = 0u; vsc < bMaxVSCtoVK;)
	{
		putwchar(L'\t');
		if (KbdTables->pusVSCtoVK[vsc] != ausVK[vsc])
		{
			auto const vkf = KbdTables->pusVSCtoVK[vsc];
			BYTE const vk = vkf & 0xFF;
			if (vk == ausVK[vsc]) wprintf_s(L"T%02hhX", vsc);
			else wprintf_s(L"%s", VK[vk]);
			Print_VK_Flag(vkf);
			putwchar(L',');
			vsc++;
		}
		else do wprintf_s(L"T%02hhX,", vsc++);  // KbdTables->pusVSCtoVK[vsc] == ausVK[vsc]
		while (vsc < bMaxVSCtoVK && vsc & 0xF && KbdTables->pusVSCtoVK[vsc] == ausVK[vsc]);
		putwchar(L'\n');
	}
	for (unsigned vsc = ARRAYSIZE(ausVK); vsc < KbdTables->bMaxVSCtoVK; vsc++)
	{
		wprintf_s(L"\t%s", VK[KbdTables->pusVSCtoVK[vsc] & 0xFF]);
		Print_VK_Flag(KbdTables->pusVSCtoVK[vsc]);
		_putws(L",");
	}
	_putws(L"\n};\n");

	_putws(L"static ALLOC_SECTION_LDATA VSC_VK aE0VscToVk[] = {");
	for (auto p = KbdTables->pVSCtoVK_E0; p->Vsc && p->Vk; p++)
	{
		auto const vsc = p->Vsc;
		auto const vk  = p->Vk & 0xFF;
		wprintf_s(L"	0x%02hhX, ", vsc);

		auto const pE0 = std::lower_bound(ausVK_E0, ausVK_E0 + ARRAYSIZE(ausVK_E0), vsc << 8);
		if (pE0 != ausVK_E0 + ARRAYSIZE(ausVK_E0) &&
			vsc == *pE0 >> 8 &&
			vk == (*pE0 & 0xFF))
			wprintf_s(L"X%02hhX", vsc);
		else wprintf_s(L"%s", VK[vk]);
		Print_VK_Flag(p->Vk);
		_putws(L",");
	}
	_putws(L"\t0\n};\n");

	_putws(L"static ALLOC_SECTION_LDATA VSC_VK aE1VscToVk[] = {");
	for (auto p = KbdTables->pVSCtoVK_E1; p->Vsc && p->Vk; p++)
	{
		switch (p->Vsc)
		{
		case 0x1D: wprintf_s(L"	0x1D, Y1D"); break;
		default: wprintf_s(L"	0x%02hhX, %s", p->Vsc, VK[p->Vk & 0xFF]); break;
		}
		Print_VK_Flag(p->Vk);
		_putws(L",");
	}
	_putws(L"\t0\n};\n");

	BYTE Mod[256]{};
	if (auto const aModification = KbdTables->pCharModifiers)
	{
		_putws(LR"(
/***************************************************************************\
* aVkToBits[]  - map Virtual Keys to Modifier Bits
*
* See kbd.h for a full description.
*
* The keyboard has only three shifter keys:
*     SHIFT (L & R) affects alphabnumeric keys,
*     CTRL  (L & R) is used to generate control characters
*     ALT   (L & R) used for generating characters by number with numpad
\***************************************************************************/
static ALLOC_SECTION_LDATA VK_TO_BIT aVkToBits[] = {)");
		auto c0 = 0;
		for (auto p = aModification->pVkToBit; p->Vk; p++)
			if (auto const len = (int)wcslen(VK[p->Vk]); c0 < len) c0 = len;
		for (auto p = aModification->pVkToBit; p->Vk; p++)
		{
			wprintf_s(L"	%*s, ", -c0, VK[p->Vk]);
			switch (p->ModBits)
			{
			case KBDSHIFT: _putws(L"KBDSHIFT,"); break;
			case KBDCTRL : _putws(L"KBDCTRL," ); break;
			case KBDALT  : _putws(L"KBDALT,"  ); break;
			case KBDKANA : _putws(L"KBDKANA," ); break;
			case KBDROYA : _putws(L"KBDROYA," ); break;
			case KBDLOYA : _putws(L"KBDLOYA," ); break;
			default: wprintf_s(L"%#hhx,\n", p->ModBits);
			}
		}
		_putws(LR"(	0
};

/***************************************************************************\
* aModification[]  - map character modifier bits to modification number
*
* See kbd.h for a full description.
*
\***************************************************************************/

static ALLOC_SECTION_LDATA MODIFIERS aModification = {
	aVkToBits,)");
		wprintf_s(L"	%hhu,", aModification->wMaxModBits);
		_putws(LR"(
	{
	//	Modification# // Keys Pressed
	//	============= // =============)");
		for (auto i = 0u; i <= aModification->wMaxModBits; i++)
		{
			auto const num = aModification->ModNumber[i];
			if (num != SHFT_INVALID) Mod[num] = i;
			if (num == SHFT_INVALID) wprintf_s(L"\t\tSHFT_INVALID, //");
			else wprintf_s(L"\t\t%-12hhu, //", num);

			auto j = 0u;
			for (auto b = i; b; b >>= 1)
			{
				switch (aModification->pVkToBit[j++].ModBits)
				{
				case KBDSHIFT    : if (b & 1) wprintf_s(L" SHFT"     ); else wprintf_s(L"%*s", (unsigned)sizeof "SHFT"     , L""); break;
				case KBDCTRL     : if (b & 1) wprintf_s(L" CTRL"     ); else wprintf_s(L"%*s", (unsigned)sizeof "CTRL"     , L""); break;
				case KBDALT      : if (b & 1) wprintf_s(L" ALT"      ); else wprintf_s(L"%*s", (unsigned)sizeof "ALT"      , L""); break;
				case KBDKANA     : if (b & 1) wprintf_s(L" KANA"     ); else wprintf_s(L"%*s", (unsigned)sizeof "KANA"     , L""); break;
				case KBDROYA     : if (b & 1) wprintf_s(L" ROYA"     ); else wprintf_s(L"%*s", (unsigned)sizeof "ROYA"     , L""); break;
				case KBDLOYA     : if (b & 1) wprintf_s(L" LOYA"     ); else wprintf_s(L"%*s", (unsigned)sizeof "LOYA"     , L""); break;
				case KBDGRPSELTAP: if (b & 1) wprintf_s(L" GRPSELTAP"); else wprintf_s(L"%*s", (unsigned)sizeof "GRPSELTAP", L""); break;
				}
			}

			putwchar('\n');
		}
		_putws(L"	}\n}");
	}

	if (auto pVkToWcharTable = KbdTables->pVkToWcharTable)
	{
		_putws(LR"(
/***************************************************************************\
*
* aVkToWchI[]  - Virtual Key to WCHAR translation for Ith shift states
*
* Table attributes: Unordered Scan, null-terminated
*
* Search this table for an entry with a matching Virtual Key to find the
* corresponding unshifted and shifted WCHAR characters.
*
* Special values for VirtualKey (column 1)
*     0xff          - dead chars for the previous entry
*     0             - terminate the list
*
* Special values for Attributes (column 2)
*     CAPLOK bit    - CAPS-LOCK affect this key like SHIFT
*
* Special values for wch[*] (column 3 & 4)
*     WCH_NONE      - No character
*     WCH_DEAD      - Dead Key (diaresis) or invalid (US keyboard has none)
*     WCH_LGTR      - Ligature (generates multiple characters)
*
\***************************************************************************/
)");
		for (auto n = 0u; pVkToWcharTable->pVkToWchars; pVkToWcharTable++)
		{
			auto const cbSize         = pVkToWcharTable->cbSize;
			auto const nModifications = pVkToWcharTable->nModifications;
			wprintf_s(L"\nstatic ALLOC_SECTION_LDATA VK_TO_WCHARS%hhu aVkToWch%u[] = {\n", nModifications, n++);

			int c0[2]{}; auto c = 0u; unsigned nMod[2]{};
			for (auto p = pVkToWcharTable->pVkToWchars; p->VirtualKey; (ULONG_PTR&)p += cbSize)
			{
				if (p->VirtualKey == VK__none_) c = 1;
				auto const len = (int)wcslen(VK[p->VirtualKey]);
				if (c0[c] < len) c0[c] = len;
				auto count = nModifications;
				if ((p->Attributes & SGCAPS) == 0) while (count > 0)
					if (p->wch[count - 1] && p->wch[count - 1] != WCH_NONE) break;
					else count--;
				if (nMod[c] < count) nMod[c] = count;
				if (p->Attributes & SGCAPS) c = 1; else c = 0;
			}

			wprintf_s(L"//\t %*s        │", c0[0], L"");
			for (auto i = 0u; i < nMod[0]; i++) Print_Modification(Mod[i]);
			if (c0[1]) wprintf_s(L" \t %*s        │", c0[1], L"");
			if (c0[1]) for (auto i = 0u; i < nMod[1]; i++) Print_Modification(Mod[i]);
			putwchar(L'\n');

			wprintf_s(L"//\t %*s        ╞", c0[0], L"");
			for (auto i = 0u; i < nMod[0] - 1; i++) wprintf_s(L"══════════╪");
			wprintf_s(L"══════════╡");
			if (c0[1]) wprintf_s(L" \t %*s        ╞", c0[1], L"");
			if (c0[1]) for (auto i = 0u; i < nMod[1] - 1; i++) wprintf_s(L"══════════╪");
			if (c0[1]) wprintf_s(L"══════════╡");
			putwchar(L'\n');

			for (auto p = pVkToWcharTable->pVkToWchars; p->VirtualKey; (ULONG_PTR&)p += cbSize)
			{
				if (p->VirtualKey == VK__none_) c = 1;
				wprintf_s(L"	{%*s, ", -c0[c], VK[p->VirtualKey]);
				if (p->Attributes & SGCAPS) c++; else c = 0;

				auto attr = p->Attributes;
				if  (attr == 0) wprintf_s(L"0     ");
				if  (attr & CAPLOK     ) { wprintf_s(L"CAPLOK"     ); if (attr &= ~CAPLOK     ) wprintf_s(L" | "); }
				if  (attr & SGCAPS     ) { wprintf_s(L"SGCAPS"     ); if (attr &= ~SGCAPS     ) wprintf_s(L" | "); }
				if  (attr & CAPLOKALTGR) { wprintf_s(L"CAPLOKALTGR"); if (attr &= ~CAPLOKALTGR) wprintf_s(L" | "); }
				if  (attr & KANALOK    ) { wprintf_s(L"KANALOK"    ); if (attr &= ~KANALOK    ) wprintf_s(L" | "); }
				if  (attr & GRPSELTAP  ) { wprintf_s(L"GRPSELTAP"  ); if (attr &= ~GRPSELTAP  ) wprintf_s(L" | "); }
				if  (attr) wprintf_s(L"%#hhx", attr);

				auto dead = false;
				auto count = nModifications;
				if ((p->Attributes & SGCAPS) == 0) while (count > 0)
					if (p->wch[count - 1] && p->wch[count - 1] != WCH_NONE) break;
					else count--;
				for (auto i = 0; i < count; i++)
				{
					auto const wch = p->wch[i];
					switch (wch)
					{
					case WCH_DEAD: wprintf_s(L", WCH_DEAD "); dead = true; continue;
					case WCH_LGTR: wprintf_s(L", WCH_LGTR "); continue;
					case WCH_NONE: wprintf_s(L", WCH_NONE "); continue;
					case '\0'    : wprintf_s(L", 0        "); continue;
					case '\\'    : wprintf_s(L", '\\\\'     "); continue;
					case '\"'    : wprintf_s(L", '\\\"'     "); continue;
					case '\''    : wprintf_s(L", '\\''     "); continue;
					case '\b'    : wprintf_s(L",  '\\b'    "); continue;
					case '\t'    : wprintf_s(L",  '\\t'    "); continue;
					case '\r'    : wprintf_s(L",  '\\r'    "); continue;
					case '\n'    : wprintf_s(L",  '\\n'    "); continue;
					case '\f'    : wprintf_s(L",  '\\f'    "); continue;
					case '\v'    : wprintf_s(L",  '\\v'    "); continue;
					case '\a'    : wprintf_s(L",  '\\a'    "); continue;
					case '\x7F'  : wprintf_s(L",  '\\x7F'  "); continue;
					case '\x98'  : wprintf_s(L",  '\\x98'  "); continue;
					case '\xA0'  : wprintf_s(L",  '\\xA0'  "); continue; // неразрывающій пробел
					case '\xAD'  : wprintf_s(L",  '\\xAD'  "); continue; // мягкій перенос
					}
					if (' ' <= wch && wch <= 0x7F)
						wprintf_s(L",  '%c'     ",  wch);
					else if (0x80 <= wch && wch <= 0xFF && cp1251)
						wprintf_s(L", L'%c'     ",  wch);
					else if (wch < 8)
						wprintf_s(L",  '\\%x'    ", wch);
					else if (wch < 0x20)
						wprintf_s(L",  '\\x%04x'", wch);
					else if (unicode)
						wprintf_s(L", L'%c'     ",  wch);
					else
						wprintf_s(L", L'\\x%04x'", wch);
				}
				wprintf_s(L"},");
				if ((p->Attributes & SGCAPS) == 0 && !dead) putwchar(L'\n');
			}
			_putws(L"\t{0}\n};");
		}

		auto n = 0u;
		_putws(L"\nstatic ALLOC_SECTION_LDATA VK_TO_WCHAR_TABLE aVkToWcharTable[] = {");
		for (auto p = KbdTables->pVkToWcharTable; p->pVkToWchars; p++, n++)
			wprintf_s(L"\tVK_TO_WCHAR_TABLE_ENTRY(aVkToWch%u)},\n", n);
		_putws(L"\t{NULL}\n};\n");
	}

	_putws(LR"(
/***************************************************************************\
* aKeyNames[], aKeyNamesExt[]  - Virtual Scancode to Key Name tables
*
* Table attributes: Ordered Scan (by scancode), null-terminated
*
* Only the names of Extended, NumPad, Dead and Non-Printable keys are here.
* (Keys producing printable characters are named by that character)
\***************************************************************************/

static ALLOC_SECTION_LDATA VSC_LPWSTR aKeyNames[] = {)");
	for (auto p = KbdTables->pKeyNames; p->vsc && p->pwsz; p++)
		wprintf_s(L"\t0x%02hhx, L\"%s\",\n", p->vsc, p->pwsz);
	_putws(LR"(	0
};

static ALLOC_SECTION_LDATA VSC_LPWSTR aKeyNamesExt[] = {)");
	for (auto p = KbdTables->pKeyNamesExt; p->vsc && p->pwsz; p++)
		wprintf_s(L"\t0x%02hhx, L\"%s\",\n", p->vsc, p->pwsz);
	_putws(L"\t0\n};");

	if (auto p = KbdTables->pKeyNamesDead)
	{
		_putws(L"\nstatic ALLOC_SECTION_LDATA DEADKEY_LPWSTR aKeyNamesDead[] = {");
		while (*p) wprintf_s(L"\tL\"%s\",\n", *p++);
		_putws(L"\tNULL\n};\n");
	}

	if (auto p = KbdTables->pDeadKey)
	{
		_putws(L"\nstatic ALLOC_SECTION_LDATA DEADKEY aDeadKey[] = {");
		for (auto accent = L'\0'; p->dwBoth; p++)
		{
			if (accent && accent != p->dwBoth >> 16) putwchar(L'\n');
			accent = p->dwBoth >> 16;
			wprintf_s(L"\tDEADTRANS(L'%c', L'%c', L'%c', ", (WCHAR)p->dwBoth, accent, p->wchComposed);
			if (p->uFlags & DKF_DEAD) _putws(L"DKF_DEAD),");
			else if (p->uFlags) wprintf_s(L"%#hx),\n", p->uFlags);
			else _putws(L"0),");
		}
		_putws(L"\t0\n};\n");
	}

	if (auto p = KbdTables->pLigature)
	{
		_putws(L"\nstatic ALLOC_SECTION_LDATA LIGATURE2 aLigature[] = {");
		for (; p->VirtualKey; (ULONG_PTR&)p += KbdTables->cbLgEntry)
		{
			wprintf_s(L"\t{%s, %hu", VK[p->VirtualKey], p->ModificationNumber);
			for (auto i = 0u; i < KbdTables->nLgMax; i++)
				wprintf_s(L", L'%c'", p->wch[i]);
			_putws(L"},");
		}
		_putws(L"\t'\\0'\n};\n");
	}

	_putws(LR"(
static ALLOC_SECTION_LDATA KBDTABLES KbdTables = {
	/*
	 * Modifier keys
	 */
	&aModification,

	/*
	 * Characters tables
	 */
	aVkToWcharTable,

	/*
	 * Diacritics
	 */)");
	_putws(KbdTables->pDeadKey ? L"\taDeadKey," : L"\tNULL,");
	_putws(LR"(
	/*
	 * Names of Keys
	 */
	aKeyNames,
	aKeyNamesExt,)");
	_putws(KbdTables->pKeyNamesDead ? L"\taKeyNamesDead," : L"\tNULL,");
	_putws(LR"(
	/*
	 * Scan codes to Virtual Keys
	 */
	ausVK, sizeof(ausVK) / sizeof(ausVK[0]),
	aE0VscToVk,
	aE1VscToVk,

	/*
	 * Locale-specific special processing
	 */)");
	putwchar(L'\t');
	auto const KbdVersion = GET_KBD_VERSION(KbdTables);
	if (KbdVersion) wprintf_s(L"MAKELONG(");
	if (auto kllf = KbdTables->fLocaleFlags & 0xFFFF)
	{
		if  (kllf & KLLF_ALTGR    ) { wprintf_s(L"KLLF_ALTGR"    ); if (kllf &= ~KLLF_ALTGR    ) wprintf_s(L" | "); }
		if  (kllf & KLLF_SHIFTLOCK) { wprintf_s(L"KLLF_SHIFTLOCK"); if (kllf &= ~KLLF_SHIFTLOCK) wprintf_s(L" | "); }
		if  (kllf & KLLF_LRM_RLM  ) { wprintf_s(L"KLLF_LRM_RLM"  ); if (kllf &= ~KLLF_LRM_RLM  ) wprintf_s(L" | "); }
	} else putwchar(L'0');
	if (KbdVersion == KBD_VERSION && KBD_VERSION) _putws(L", KBD_VERSION),"); else wprintf_s(L", %hu),\n", KbdVersion);
	_putws(LR"(
	/*
	 * Ligatures
	 */)");
	if (KbdTables->pLigature)
	{
		wprintf_s(L"\t%hu", KbdTables->nLgMax);
		_putws(LR"(,
	sizeof(aLigature[0]),
	(PLIGATURE1)aLigature)");
	}
	else _putws(L"\t0, 0, NULL");

	_putws(LR"(
	/*
	 * Type
	 */)");
	switch (KbdTables->dwType)
	{
	case NLSKBD_OEM_MICROSOFT : _putws(L"\tNLSKBD_OEM_MICROSOFT,"); break;
	case NLSKBD_OEM_AX        : _putws(L"\tNLSKBD_OEM_AX,"); break;
	case NLSKBD_OEM_EPSON     : _putws(L"\tNLSKBD_OEM_EPSON,"); break;
	case NLSKBD_OEM_FUJITSU   : _putws(L"\tNLSKBD_OEM_FUJITSU,"); break;
	case NLSKBD_OEM_IBM       : _putws(L"\tNLSKBD_OEM_IBM,"); break;
	case NLSKBD_OEM_MATSUSHITA: _putws(L"\tNLSKBD_OEM_MATSUSHITA,"); break;
	case NLSKBD_OEM_NEC       : _putws(L"\tNLSKBD_OEM_NEC,"); break;
	case NLSKBD_OEM_TOSHIBA   : _putws(L"\tNLSKBD_OEM_TOSHIBA,"); break;
	case NLSKBD_OEM_DEC       : _putws(L"\tNLSKBD_OEM_DEC,"); break;
	default: wprintf_s(L"\t%u", KbdTables->dwType);
	}
	switch (KbdTables->dwSubType)
	{
	case NLSKBD_OEM_MICROSOFT:
		if (KbdTables->dwType == KEYBOARD_TYPE_KOREA)
		{
			switch (KbdTables->dwSubType)
			{
			case MICROSOFT_KBD_101A_TYPE: _putws(L"\tMICROSOFT_KBD_101A_TYPE,"); break;
			case MICROSOFT_KBD_101B_TYPE: _putws(L"\tMICROSOFT_KBD_101B_TYPE,"); break;
			case MICROSOFT_KBD_101C_TYPE: _putws(L"\tMICROSOFT_KBD_101C_TYPE,"); break;
			case MICROSOFT_KBD_103_TYPE : _putws(L"\tMICROSOFT_KBD_103_TYPE,"); break;
			default: wprintf_s(L"\t%u\n", KbdTables->dwSubType);
			}
		} else switch (KbdTables->dwSubType)
		{
		case MICROSOFT_KBD_101_TYPE: _putws(L"\tMICROSOFT_KBD_101_TYPE,"); break;
		case MICROSOFT_KBD_AX_TYPE : _putws(L"\tMICROSOFT_KBD_AX_TYPE,"); break;
		case MICROSOFT_KBD_106_TYPE: _putws(L"\tMICROSOFT_KBD_106_TYPE,"); break;
		case MICROSOFT_KBD_002_TYPE: _putws(L"\tMICROSOFT_KBD_002_TYPE,"); break;
		case MICROSOFT_KBD_001_TYPE: _putws(L"\tMICROSOFT_KBD_001_TYPE,"); break;
		case MICROSOFT_KBD_FUNC    : _putws(L"\tMICROSOFT_KBD_FUNC,"); break;
		default: wprintf_s(L"\t%u\n", KbdTables->dwSubType);
		}
		break;
	case NLSKBD_OEM_AX:
		switch (KbdTables->dwSubType)
		{
		case AX_KBD_DESKTOP_TYPE:
		case MICROSOFT_KBD_101_TYPE : _putws(L"\tMICROSOFT_KBD_101_TYPE,"); break;
		default: wprintf_s(L"\t%u\n", KbdTables->dwSubType);
		}
		break;
	case NLSKBD_OEM_FUJITSU:
		switch (KbdTables->dwSubType)
		{
		case FMR_KBD_JIS_TYPE  : _putws(L"\tFMR_KBD_JIS_TYPE,"); break;
		case FMR_KBD_OASYS_TYPE: _putws(L"\tFMR_KBD_OASYS_TYPE,"); break;
		case FMV_KBD_OASYS_TYPE: _putws(L"\tFMV_KBD_OASYS_TYPE,"); break;
		default: wprintf_s(L"\t%u\n", KbdTables->dwSubType);
		}
		break;
	case NLSKBD_OEM_NEC:
		switch (KbdTables->dwSubType)
		{
		case NEC_KBD_NORMAL_TYPE: _putws(L"\tNEC_KBD_NORMAL_TYPE,"); break;
		case NEC_KBD_N_MODE_TYPE: _putws(L"\tNEC_KBD_N_MODE_TYPE,"); break;
		case NEC_KBD_H_MODE_TYPE: _putws(L"\tNEC_KBD_H_MODE_TYPE,"); break;
		case NEC_KBD_LAPTOP_TYPE: _putws(L"\tNEC_KBD_LAPTOP_TYPE,"); break;
		case NEC_KBD_106_TYPE   : _putws(L"\tNEC_KBD_106_TYPE,"); break;
		default: wprintf_s(L"\t%u\n", KbdTables->dwSubType);
		}
		break;
	case NLSKBD_OEM_TOSHIBA:
		switch (KbdTables->dwSubType)
		{
		case TOSHIBA_KBD_DESKTOP_TYPE: _putws(L"\tTOSHIBA_KBD_DESKTOP_TYPE,"); break;
		case TOSHIBA_KBD_LAPTOP_TYPE : _putws(L"\tTOSHIBA_KBD_LAPTOP_TYPE,"); break;
		default: wprintf_s(L"\t%u\n", KbdTables->dwSubType);
		}
		break;
	case NLSKBD_OEM_DEC:
		switch (KbdTables->dwSubType)
		{
		case DEC_KBD_ANSI_LAYOUT_TYPE: _putws(L"\tDEC_KBD_ANSI_LAYOUT_TYPE,"); break;
		case DEC_KBD_JIS_LAYOUT_TYPE : _putws(L"\tDEC_KBD_JIS_LAYOUT_TYPE,"); break;
		default: wprintf_s(L"\t%u\n", KbdTables->dwSubType);
		}
		break;
	default: wprintf_s(L"\t%u\n", KbdTables->dwSubType);
	}

	_putws(LR"(
};

PKBDTABLES KbdLayerDescriptor(VOID)
{
#ifdef _DEBUG
	__debugbreak();
#endif
	return &KbdTables;
})");

	if (KbdNlsTables)
	{
		_putws(LR"(
/***********************************************************************\
* VkToFuncTable[]
*
\***********************************************************************/
static ALLOC_SECTION_LDATA VK_F VkToFuncTable[] = {)");
		for (auto i = 0u; i < KbdNlsTables->NumOfVkToF; i++)
		{
			_putws(L"\t{");
			const auto& VkToF = KbdNlsTables->pVkToF[i];
			switch (VkToF.Vk)
			{
			case VK_DBE_ALPHANUMERIC          : _putws(L"\t\tVK_DBE_ALPHANUMERIC,"          ); break;
			case VK_DBE_KATAKANA              : _putws(L"\t\tVK_DBE_KATAKANA,"              ); break;
			case VK_DBE_HIRAGANA              : _putws(L"\t\tVK_DBE_HIRAGANA,"              ); break;
			case VK_DBE_SBCSCHAR              : _putws(L"\t\tVK_DBE_SBCSCHAR,"              ); break;
			case VK_DBE_DBCSCHAR              : _putws(L"\t\tVK_DBE_DBCSCHAR,"              ); break;
			case VK_DBE_ROMAN                 : _putws(L"\t\tVK_DBE_ROMAN,"                 ); break;
			case VK_DBE_NOROMAN               : _putws(L"\t\tVK_DBE_NOROMAN,"               ); break;
			case VK_DBE_ENTERWORDREGISTERMODE : _putws(L"\t\tVK_DBE_ENTERWORDREGISTERMODE," ); break;
			case VK_DBE_ENTERIMECONFIGMODE    : _putws(L"\t\tVK_DBE_ENTERIMECONFIGMODE,"    ); break;
			case VK_DBE_FLUSHSTRING           : _putws(L"\t\tVK_DBE_FLUSHSTRING,"           ); break;
			case VK_DBE_CODEINPUT             : _putws(L"\t\tVK_DBE_CODEINPUT,"             ); break;
			case VK_DBE_NOCODEINPUT           : _putws(L"\t\tVK_DBE_NOCODEINPUT,"           ); break;
			case VK_DBE_DETERMINESTRING       : _putws(L"\t\tVK_DBE_DETERMINESTRING,"       ); break;
			case VK_DBE_ENTERDLGCONVERSIONMODE: _putws(L"\t\tVK_DBE_ENTERDLGCONVERSIONMODE,"); break;
			default: wprintf_s(L"\t\t%s,\n", VK[VkToF.Vk]); break;
			}
			switch (VkToF.NLSFEProcCurrent)
			{
			case KBDNLS_INDEX_NORMAL: _putws(L"\t\tKBDNLS_INDEX_NORMAL,"); break;
			case KBDNLS_INDEX_ALT   : _putws(L"\t\tKBDNLS_INDEX_ALT,"); break;
			default: wprintf_s(L"\t\t%hhu,\n", VkToF.NLSFEProcCurrent); break;
			}
			switch (VkToF.NLSFEProcType)
			{
			case KBDNLS_TYPE_NULL  : wprintf_s(L"\t\tKBDNLS_TYPE_NULL, 0b"); break;
			case KBDNLS_TYPE_NORMAL: wprintf_s(L"\t\tKBDNLS_TYPE_NORMAL, 0b"); break;
			case KBDNLS_TYPE_TOGGLE: wprintf_s(L"\t\tKBDNLS_TYPE_TOGGLE, 0b"); break;
			default: wprintf_s(L"\t\t%hhu, 0b", VkToF.NLSFEProcType); break;
			}
			for (int j = 7; j >= 0; j--)
				putwchar('0' + (VkToF.NLSFEProcSwitch >> j & 1));
			_putws(L",\n\t\t{");
			for (auto j = 0u; j < ARRAYSIZE(VkToF.NLSFEProc); j++)
			{
				wprintf_s(L"\t\t\t{");
				auto const& NLSFEProc = VkToF.NLSFEProc[j];
				Print_NLSFEProcIndex(NLSFEProc.NLSFEProcIndex);
				if ((NLSFEProc.NLSFEProcIndex == KBDNLS_SEND_PARAM_VK ||
					 NLSFEProc.NLSFEProcIndex == KBDNLS_KANAEVENT) &&
					 NLSFEProc.NLSFEProcParam < ARRAYSIZE(VK))
					 wprintf_s(L", %s},\n", VK[NLSFEProc.NLSFEProcParam]);
				else wprintf_s(L", %u},\n",    NLSFEProc.NLSFEProcParam);
			}
			_putws(L"\t\t},{");
			for (auto j = 0u; j < ARRAYSIZE(VkToF.NLSFEProc); j++)
			{
				wprintf_s(L"\t\t\t{");
				auto const& NLSFEProc = VkToF.NLSFEProc[j];
				Print_NLSFEProcIndex(NLSFEProc.NLSFEProcIndex);
				if ((NLSFEProc.NLSFEProcIndex == KBDNLS_SEND_PARAM_VK ||
					 NLSFEProc.NLSFEProcIndex == KBDNLS_KANAEVENT) &&
					 NLSFEProc.NLSFEProcParam < ARRAYSIZE(VK))
					 wprintf_s(L", %s},\n", VK[NLSFEProc.NLSFEProcParam]);
				else wprintf_s(L", %u},\n",    NLSFEProc.NLSFEProcParam);
			}
			_putws(L"\t\t}\n\t},");
		}
		_putws(LR"(};

/***********************************************************************\
* KbdNlsTables
*
\***********************************************************************/
ALLOC_SECTION_LDATA KBDNLSTABLES KbdNlsTables = {)");
		switch (KbdNlsTables->OEMIdentifier)
		{
		case NLSKBD_OEM_MICROSOFT : _putws(L"\tNLSKBD_OEM_MICROSOFT,"); break;
		case NLSKBD_OEM_AX        : _putws(L"\tNLSKBD_OEM_AX,"); break;
		case NLSKBD_OEM_EPSON     : _putws(L"\tNLSKBD_OEM_EPSON,"); break;
		case NLSKBD_OEM_FUJITSU   : _putws(L"\tNLSKBD_OEM_FUJITSU,"); break;
		case NLSKBD_OEM_IBM       : _putws(L"\tNLSKBD_OEM_IBM,"); break;
		case NLSKBD_OEM_MATSUSHITA: _putws(L"\tNLSKBD_OEM_MATSUSHITA,"); break;
		case NLSKBD_OEM_NEC       : _putws(L"\tNLSKBD_OEM_NEC,"); break;
		case NLSKBD_OEM_TOSHIBA   : _putws(L"\tNLSKBD_OEM_TOSHIBA,"); break;
		case NLSKBD_OEM_DEC       : _putws(L"\tNLSKBD_OEM_DEC,"); break;
		default: wprintf_s(L"\t%hu,             // OEM ID\n", KbdNlsTables->OEMIdentifier);
		}
		putwchar(L'\t');
		if (auto layout = KbdNlsTables->LayoutInformation)
		{
			if (layout & NLSKBD_INFO_SEND_IME_NOTIFICATION) { wprintf_s(L"NLSKBD_INFO_SEND_IME_NOTIFICATION"); if (layout &= ~NLSKBD_INFO_SEND_IME_NOTIFICATION) wprintf_s(L" | "); }
			if (layout & NLSKBD_INFO_ACCESSIBILITY_KEYMAP ) { wprintf_s(L"NLSKBD_INFO_ACCESSIBILITY_KEYMAP" ); if (layout &= ~NLSKBD_INFO_ACCESSIBILITY_KEYMAP ) wprintf_s(L" | "); }
			if (layout & NLSKBD_INFO_EMURATE_101_KEYBOARD ) { wprintf_s(L"NLSKBD_INFO_EMURATE_101_KEYBOARD" ); if (layout &= ~NLSKBD_INFO_EMURATE_101_KEYBOARD ) wprintf_s(L" | "); }
			if (layout & NLSKBD_INFO_EMURATE_106_KEYBOARD ) { wprintf_s(L"NLSKBD_INFO_EMURATE_106_KEYBOARD" ); if (layout &= ~NLSKBD_INFO_EMURATE_106_KEYBOARD ) wprintf_s(L" | "); }
			if (layout) wprintf_s(L"0x%hx", layout);
			_putws(L",");
		} else _putws(L"0,             // Layout Information");
		wprintf_s(L"\t%u,             // Number of VK_F entry\n", KbdNlsTables->NumOfVkToF);
		wprintf_s(L"\tVkToFuncTable, // Pointer to VK_F array\n");
		wprintf_s(L"\t%d,             // Number of MouseVk entry\n", KbdNlsTables->NumOfMouseVKey);
		wprintf_s(KbdNlsTables->NumOfMouseVKey ?
		          L"\tNULL           // Pointer to MouseVk array" :
		          L"\tMouseVk        // Pointer to MouseVk array");
		_putws(LR"("
};

PKBDNLSTABLES KbdNlsLayerDescriptor(VOID)
{
		return &KbdNlsTables106;
})");
	}
}
