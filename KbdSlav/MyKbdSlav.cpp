#define KBD_TYPE KEYBOARD_TYPE_GENERIC_101

#include <Windows.h>
#include <kbd.h>

#pragma data_seg(".data")
#define ALLOC_SECTION_LDATA __declspec(allocate(".data"))

#define VK_POWER 0x5E

#define WCH_SOH '\1' // Start of heading
#define WCH_STX '\2' // Start of text
#define WCH_ETX '\3' // End   of text
#define WCH_EOT '\4' // End   of transmission
#define WCH_ENQ '\5' // Enquiry
#define WCH_ACK '\6' // Acknowledgement
#define WCH_SO  '\xE' // Shift out	«Переключиться на другую ленту (кодировку)»
#define WCH_SI  '\xF' // Shift in	«Переключиться на исходную ленту (кодировку)»
#define WCH_DLE '\x10' // Data link escape	«Экранирование канала данных»
#define WCH_DC1 '\x11' // Device control 1
#define WCH_DC2 '\x12' // Device control 2
#define WCH_DC3 '\x13' // Device control 3
#define WCH_DC4 '\x14' // Device control 4
#define WCH_NAK '\x15' // Negative acknowledgement
#define WCH_SYN '\x16' // Synchronous idle
#define WCH_ETB '\x17' // End of transmission block
#define WCH_CAN '\x18' // Cancel
#define WCH_EOM '\x19' // End of medium
#define WCH_SUB '\x1A' // Substitute
#define WCH_ESC '\x1B' // Escape
#define WCH_FS  '\x1C' // File   separator
#define WCH_GS  '\x1D' // Group  separator
#define WCH_RS  '\x1E' // Record separator
#define WCH_US  '\x1F' // Unit   separator
#define WCH_DEL '\x7F'

#define VK_TO_WCHAR_TABLE_ENTRY(aVkToWchI) \
	{(PVK_TO_WCHARS1)aVkToWchI, ARRAYSIZE(aVkToWchI[0].wch), sizeof aVkToWchI[0]}

/***************************************************************************\
* ausVK[] - Virtual Scan Code to Virtual Key conversion table
\***************************************************************************/
#if KBD_TYPE == KEYBOARD_TYPE_GENERIC_101
static ALLOC_SECTION_LDATA USHORT ausVK[] = {
	T00, T01, T02, T03, T04, T05, T06, T07, T08, T09, T0A, T0B, T0C, T0D, T0E, T0F,
	T10, T11, T12, T13, T14, T15, T16, T17, T18, T19, T1A, T1B, T1C, T1D, T1E, T1F,
	T20, T21, T22, T23, T24, T25, T26, T27, T28, T29, T2A, T2B, T2C, T2D, T2E, T2F,
	T30, T31, T32, T33, T34, T35,

	/*
	 * Right-hand Shift key must have KBDEXT bit set.
	 */
	T36 | KBDEXT,
	T37 | KBDMULTIVK,               // numpad_* + Shift/Alt -> SnapShot

	T38, T39, T3A, T3B, T3C, T3D, T3E, T3F, T40, T41, T42, T43, T44,

	/*
	 * NumLock Key:
	 *     KBDEXT     - VK_NUMLOCK is an Extended key
	 *     KBDMULTIVK - VK_NUMLOCK or VK_PAUSE (without or with CTRL)
	 */
	T45 | KBDMULTIVK | KBDEXT,
	T46 | KBDMULTIVK,

	/*
	 * Number Pad keys:
	 *     KBDNUMPAD  - digits 0-9 and decimal point.
	 *     KBDSPECIAL - require special processing by Windows
	 */
	T47 | KBDNUMPAD | KBDSPECIAL,   // Numpad 7 (Home)
	T48 | KBDNUMPAD | KBDSPECIAL,   // Numpad 8 (Up),
	T49 | KBDNUMPAD | KBDSPECIAL,   // Numpad 9 (PgUp),
	T4A,
	T4B | KBDNUMPAD | KBDSPECIAL,   // Numpad 4 (Left),
	T4C | KBDNUMPAD | KBDSPECIAL,   // Numpad 5 (Clear),
	T4D | KBDNUMPAD | KBDSPECIAL,   // Numpad 6 (Right),
	T4E,
	T4F | KBDNUMPAD | KBDSPECIAL,   // Numpad 1 (End),
	T50 | KBDNUMPAD | KBDSPECIAL,   // Numpad 2 (Down),
	T51 | KBDNUMPAD | KBDSPECIAL,   // Numpad 3 (PgDn),
	T52 | KBDNUMPAD | KBDSPECIAL,   // Numpad 0 (Ins),
	T53 | KBDNUMPAD | KBDSPECIAL,   // Numpad . (Del),

	T54, T55, T56, T57, T58, T59, T5A, T5B, T5C, T5D, T5E, T5F,
	T60, T61, T62, T63, T64, T65, T66, T67, T68, T69, T6A, T6B, T6C, T6D, T6E, T6F,
	T70, T71, T72, T73, T74, T75, T76, T77, T78, T79, T7A, T7B, T7C, T7D, T7E, T7F
};

static ALLOC_SECTION_LDATA VSC_VK aE0VscToVk[] = {
	0x10, KBDEXT | X10,      // Speedracer: Previous Track
	0x19, KBDEXT | X19,      // Speedracer: Next Track
	0x1C, KBDEXT | X1C,      // Numpad Enter
	0x1D, KBDEXT | X1D,      // RControl
	0x20, KBDEXT | X20,      // Speedracer: Volume Mute
	0x21, KBDEXT | X21,      // Speedracer: Launch App 2
	0x22, KBDEXT | X22,      // Speedracer: Media Play/Pause
	0x24, KBDEXT | X24,      // Speedracer: Media Stop
	0x2E, KBDEXT | X2E,      // Speedracer: Volume Down
	0x30, KBDEXT | X30,      // Speedracer: Volume Up
	0x32, KBDEXT | X32,      // Speedracer: Browser Home
	0x35, KBDEXT | X35,      // Numpad Divide
	0x37, KBDEXT | X37,      // Snapshot
	0x38, KBDEXT | VK_OEM_8, // Kana
	0x46, KBDEXT | X46,      // Break (Ctrl + Pause)
	0x47, KBDEXT | X47,      // Home
	0x48, KBDEXT | X48,      // Up
	0x49, KBDEXT | X49,      // Prior
	0x4B, KBDEXT | X4B,      // Left
	0x4D, KBDEXT | X4D,      // Right
	0x4F, KBDEXT | X4F,      // End
	0x50, KBDEXT | X50,      // Down
	0x51, KBDEXT | X51,      // Next
	0x52, KBDEXT | X52,      // Insert
	0x53, KBDEXT | X53,      // Delete
	0x5B, KBDEXT | X5B,      // Left Win
	0x5C, KBDEXT | X5C,      // Right Win
	0x5D, KBDEXT | X5D,      // Application
	0x5F, KBDEXT | X5F,      // Speedracer: Sleep
	0x65, KBDEXT | X65,      // Speedracer: Browser Search
	0x66, KBDEXT | X66,      // Speedracer: Browser Favorites
	0x67, KBDEXT | X67,      // Speedracer: Browser Refresh
	0x68, KBDEXT | X68,      // Speedracer: Browser Stop
	0x69, KBDEXT | X69,      // Speedracer: Browser Forward
	0x6A, KBDEXT | X6A,      // Speedracer: Browser Back
	0x6B, KBDEXT | X6B,      // Speedracer: Launch App 1
	0x6C, KBDEXT | X6C,      // Speedracer: Launch Mail
	0x6D, KBDEXT | X6D,      // Speedracer: Launch Media Selector
	0
};

static ALLOC_SECTION_LDATA VSC_VK aE1VscToVk[] = {
	0x1D, Y1D,  // Pause
	0
};
#endif

/***************************************************************************\
* aVkToBits[]  - map Virtual Keys to Modifier Bits
*
* The keyboard has only three shifter keys:
*     SHIFT (L & R) affects alphabnumeric keys,
*     CTRL  (L & R) is used to generate control characters
*     ALT   (L & R) used for generating characters by number with numpad
\***************************************************************************/
static ALLOC_SECTION_LDATA VK_TO_BIT aVkToBits[] = {
	VK_SHIFT,       KBDSHIFT,
	VK_CONTROL,     KBDCTRL,
	VK_MENU,        KBDALT,
	VK_OEM_8,       KBDKANA,
//	VK_OEM_FJ_ROYA, KBDROYA,
//	VK_OEM_FJ_LOYA, KBDLOYA,
	0
};

/***************************************************************************\
* aModification[]  - map character modifier bits to modification number
*
\***************************************************************************/
static ALLOC_SECTION_LDATA MODIFIERS CharModifiers = {
	&aVkToBits[0],
	9,
	{
	//  /*      */ Modification# Shift         // Keys Pressed
	//  /* ════ */ ════════════, ════════════, /* ════════════════════ */
		/*      */ 0           , 1           , /*        	           */
		/*      */ 2           , SHFT_INVALID, /* Control	           */
		/*      */ SHFT_INVALID, SHFT_INVALID, /* Alt    	           */
		/*      */ SHFT_INVALID, SHFT_INVALID, /* AltGr  	           */
		/* Kana */ 3           , 4           , /*        	           */
	//	/* Kana */ SHFT_INVALID, SHFT_INVALID, /* Control	           */
	//	/* Kana */ SHFT_INVALID, SHFT_INVALID, /* Alt    	           */
	//	/* Kana */ SHFT_INVALID, SHFT_INVALID, /* AltGr  	           */
	//	/*      */ SHFT_INVALID, SHFT_INVALID, /*        	      Roya */
	//	/*      */ SHFT_INVALID, SHFT_INVALID, /* Control	      Roya */
	//	/*      */ SHFT_INVALID, SHFT_INVALID, /* Alt    	      Roya */
	//	/*      */ SHFT_INVALID, SHFT_INVALID, /* AltGr  	      Roya */
	//	/* Kana */ SHFT_INVALID, SHFT_INVALID, /*        	      Roya */
	//	/* Kana */ SHFT_INVALID, SHFT_INVALID, /* Control	      Roya */
	//	/* Kana */ SHFT_INVALID, SHFT_INVALID, /* Alt    	      Roya */
	//	/* Kana */ SHFT_INVALID, SHFT_INVALID, /* AltGr  	      Roya */
	//	/*      */ SHFT_INVALID, SHFT_INVALID, /*        	 Loya      */
	//	/*      */ SHFT_INVALID, SHFT_INVALID, /* Control	 Loya      */
	//	/*      */ SHFT_INVALID, SHFT_INVALID, /* Alt    	 Loya      */
	//	/*      */ SHFT_INVALID, SHFT_INVALID, /* AltGr  	 Loya      */
	//	/* Kana */ SHFT_INVALID, SHFT_INVALID, /*        	 Loya      */
	//	/* Kana */ SHFT_INVALID, SHFT_INVALID, /* Control	 Loya      */
	//	/* Kana */ SHFT_INVALID, SHFT_INVALID, /* Alt    	 Loya      */
	//	/* Kana */ SHFT_INVALID, SHFT_INVALID, /* AltGr  	 Loya      */
	//	/*      */ SHFT_INVALID, SHFT_INVALID, /*        	 Loya Roya */
	//	/*      */ SHFT_INVALID, SHFT_INVALID, /* Control	 Loya Roya */
	//	/*      */ SHFT_INVALID, SHFT_INVALID, /* Alt    	 Loya Roya */
	//	/*      */ SHFT_INVALID, SHFT_INVALID, /* AltGr  	 Loya Roya */
	//	/* Kana */ SHFT_INVALID, SHFT_INVALID, /*        	 Loya Roya */
	//	/* Kana */ SHFT_INVALID, SHFT_INVALID, /* Control	 Loya Roya */
	//	/* Kana */ SHFT_INVALID, SHFT_INVALID, /* Alt    	 Loya Roya */
	//	/* Kana */ SHFT_INVALID, SHFT_INVALID, /* AltGr  	 Loya Roya */
	}
};

/***************************************************************************\
*
* aVkToWchN[]  - Virtual Key to WCHAR translation for N shift states
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
static ALLOC_SECTION_LDATA VK_TO_WCHARS2 aVkToWch0[] = {
//	          |     |Shift|
//	          |=====|=====|
	{VK_TAB, 0, '\t', '\t'},
	{0}
 };

static ALLOC_SECTION_LDATA VK_TO_WCHARS3 aVkToWch1[] = {
//	             |        |  Shift | Control|
//	             |========|========|========|
	{VK_CANCEL, 0,  '\3'  ,  '\3'  ,  '\3'  },
	{VK_BACK  , 0,  '\b'  ,  '\b'  , WCH_DEL},
	{VK_RETURN, 0,  '\r'  ,  '\r'  ,  '\n'  },
	{VK_ESCAPE, 0, WCH_ESC, WCH_ESC, WCH_ESC},
	{0}
};

static ALLOC_SECTION_LDATA VK_TO_WCHARS4 aVkToWch2[] = {
//	              │    │Shift│ Control │ Kana│
//	              ╞════╪═════╪═════════╪═════╡
	{VK_DECIMAL, 0, ',', '.' , WCH_NONE, L'…'}, // L'‥'
	{0}
};

static ALLOC_SECTION_LDATA VK_TO_WCHARS5 aVkToWch3[] = {
//	                    │    │Shift│ Control │Kana │↑Kana↑│
//	                    ╞════╪═════╪═════════╪═════╪══════╡
	{VK_DIVIDE  , SGCAPS, '/', L'÷', WCH_NONE, L'∕', L'⁄' }, {VK__none_, 0, '/', '/'},
	{VK_MULTIPLY, SGCAPS, '*', L'×', WCH_NONE, L'⋅', L'⁢'  }, {VK__none_, 0, '*', '*'},
	{VK_SUBTRACT, SGCAPS, '-', L'−', WCH_NONE, L'–', L'—' }, {VK__none_, 0, '-', '-'},
	{VK_ADD     , 0     , '+',  '+', WCH_NONE, L'±', L'∓' },
	{VK_SPACE   , 0     , ' ',  ' ',   ' '   , L' ', L' ' },
	{0}
};

static ALLOC_SECTION_LDATA VK_TO_WCHARS5 aVkToWch4[] = {
//	                      │     │Shift│ Control │  Kana   │ ↑Kana↑  │               │     │Shift│ Control │ Kana    │↑Kana↑│
//	                      ╞═════╪═════╪═════════╪═════════╪═════════╡               ╞═════╪═════╪═════════╪═════════╪══════╡
	{VK_OEM_3     , SGCAPS, L'ё', L'Ё', WCH_NONE, L'̀'    , L'³'    }, {VK__none_, 0,  '`',  '~'},
	{'1'          , SGCAPS,  '!',  '[', WCH_NONE, L'́'    , WCH_DEAD}, {VK__none_, 0,  '1',  '!', WCH_NONE, WCH_NONE, L'ᵢ'},
	{'2'          , SGCAPS, L'«',  ']', WCH_NONE, L'̏'    , '\"'    }, {VK__none_, 0,  '2',  '@'},
	{'3'          , SGCAPS, L'»', L'№', WCH_NONE, L'̄'    , L'§'    }, {VK__none_, 0,  '3',  '#'},
	{'4'          , SGCAPS, L'₽', L'€', WCH_NONE,  '$'    , L'¥'    }, {VK__none_, 0,  '4',  '$'},
	{'5'          , SGCAPS,  '%', L'‰', WCH_NONE, WCH_DEAD, L'½'    }, {VK__none_, 0,  '5',  '%', WCH_NONE, L'°'},
	{'6'          , SGCAPS, L'ѣ', L'Ѣ', WCH_NONE, L'̂'    , L'⅔'    }, {VK__none_, 0,  '6',  '^'},
	{'7'          , SGCAPS,  '?',  '{', WCH_NONE,  '&'    , L'¾'    }, {VK__none_, 0,  '7',  '&'},
	{'8'          , SGCAPS, L'×',  '}', WCH_NONE, L'∞'    , L'∏'    }, {VK__none_, 0,  '8',  '*'},
	{'9'          , SGCAPS,  '(',  '<', WCH_NONE, L'҃'    , L'≤'    }, {VK__none_, 0,  '9',  '('},
	{'0'          , SGCAPS,  ')',  '>', WCH_NONE, L'҂'    , L'≥'    }, {VK__none_, 0,  '0',  ')'},
	{VK_OEM_MINUS , SGCAPS,  '-', L'−', WCH_NONE, L'­'     , L'—'    }, {VK__none_, 0,  '-',  '_'},
	{VK_OEM_PLUS  , SGCAPS,  '=', L'≠', WCH_NONE, L'≈'    , L'≡'    }, {VK__none_, 0,  '=',  '+'},
	{'Q'          , SGCAPS, L'й', L'Й', WCH_NONE, L'ј'    , L'Ј'    }, {VK__none_, 0,  'q',  'Q'},
	{'W'          , SGCAPS, L'ц', L'Ц', WCH_NONE, L'џ'    , L'Џ'    }, {VK__none_, 0,  'w',  'W'},
	{'E'          , SGCAPS, L'у', L'У', WCH_NONE, L'ѫ'    , L'Ѫ'    }, {VK__none_, 0,  'e',  'E'},
	{'R'          , SGCAPS, L'к', L'К', WCH_NONE, L'ќ'    , L'Ќ'    }, {VK__none_, 0,  'r',  'R'},
	{'T'          , SGCAPS, L'е', L'Е', WCH_NONE, L'ѣ'    , L'Ѣ'    }, {VK__none_, 0,  't',  'T'},
	{'Y'          , SGCAPS, L'н', L'Н', WCH_NONE, L'њ'    , L'Њ'    }, {VK__none_, 0,  'y',  'Y'},
	{'U'          , SGCAPS, L'г', L'Г', WCH_NONE, L'ѓ'    , L'Ѓ'    }, {VK__none_, 0,  'u',  'U'},
	{'I'          , SGCAPS, L'ш', L'Ш', WCH_NONE, L'ї'    , L'Ї'    }, {VK__none_, 0,  'i',  'I'},
	{'O'          , SGCAPS, L'щ', L'Щ', WCH_NONE, L'ѹ'    , L'Ѹ'    }, {VK__none_, 0,  'o',  'O'},
	{'P'          , SGCAPS, L'з', L'З', WCH_NONE, L'ꙁ'    , L'Ꙁ'    }, {VK__none_, 0,  'p',  'P'},
	{VK_OEM_4     , SGCAPS, L'х', L'Х', WCH_NONE, L'ꙗ'   , L'Ꙗ'    }, {VK__none_, 0,  '[',  '{'},
	{VK_OEM_6     , SGCAPS, L'ъ', L'Ъ', WCH_NONE, L'ѩ'    , L'Ѩ'    }, {VK__none_, 0,  ']',  '}'},
	{VK_OEM_5     , SGCAPS, '\\',  '/', WCH_NONE, L'ѥ'    , L'Ѥ'    }, {VK__none_, 0, '\\',  '|'},
	{'A'          , SGCAPS, L'ф', L'Ф', WCH_NONE, L'ѳ'    , L'Ѳ'    }, {VK__none_, 0,  'a',  'A'},
	{'S'          , SGCAPS, L'ы', L'Ы', WCH_NONE, L'ѕ'    , L'Ѕ'    }, {VK__none_, 0,  's',  'S'},
	{'D'          , SGCAPS, L'в', L'В', WCH_NONE, L'ћ'    , L'Ћ'    }, {VK__none_, 0,  'd',  'D'},
	{'F'          , SGCAPS, L'а', L'А', WCH_NONE, L'ѧ'    , L'Ѧ'    }, {VK__none_, 0,  'f',  'F'},
	{'G'          , SGCAPS, L'п', L'П', WCH_NONE, L'ѱ'    , L'Ѱ'    }, {VK__none_, 0,  'g',  'G'},
	{'H'          , SGCAPS, L'р', L'Р', WCH_NONE, L'ѡ'    , L'Ѡ'    }, {VK__none_, 0,  'h',  'H'},
	{'J'          , SGCAPS, L'о', L'О', WCH_NONE, L'ꙋ'    , L'Ꙋ'    }, {VK__none_, 0,  'j',  'J'},
	{'K'          , SGCAPS, L'л', L'Л', WCH_NONE, L'љ'    , L'Љ'    }, {VK__none_, 0,  'k',  'K'},
	{'L'          , SGCAPS, L'д', L'Д', WCH_NONE, L'ѕ'    , L'Ѕ'    }, {VK__none_, 0,  'l',  'L'},
	{VK_OEM_1     , SGCAPS, L'ж', L'Ж', WCH_NONE, L'ꙃ'    , L'Ꙃ'    }, {VK__none_, 0,  ';',  ':'},
	{VK_OEM_7     , SGCAPS, L'э', L'Э', WCH_NONE, L'є'    , L'Є'    }, {VK__none_, 0, '\'', '\"'},
	{'Z'          , SGCAPS, L'я', L'Я', WCH_NONE, L'ђ'    , L'Ђ'    }, {VK__none_, 0,  'z',  'Z'},
	{'X'          , SGCAPS, L'ч', L'Ч', WCH_NONE, L'ѯ'    , L'Ѯ'    }, {VK__none_, 0,  'x',  'X'},
	{'C'          , SGCAPS, L'с', L'С', WCH_NONE, L'ҁ'    , L'Ҁ'    }, {VK__none_, 0,  'c',  'C'},
	{'V'          , SGCAPS, L'м', L'М', WCH_NONE, L'ў'    , L'Ў'    }, {VK__none_, 0,  'v',  'V'},
	{'B'          , SGCAPS, L'и', L'И', WCH_NONE, L'і'    , L'І'    }, {VK__none_, 0,  'b',  'B'},
	{'N'          , SGCAPS, L'т', L'Т', WCH_NONE, L'ѿ'    , L'Ѿ'    }, {VK__none_, 0,  'n',  'N'},
	{'M'          , SGCAPS, L'ь', L'Ь', WCH_NONE, L'ѵ'    , L'Ѵ'    }, {VK__none_, 0,  'm',  'M'},
	{VK_OEM_COMMA , SGCAPS, L'б', L'Б', WCH_NONE, L'ѭ'    , L'Ѭ'    }, {VK__none_, 0,  ',',  '<'},
	{VK_OEM_PERIOD, SGCAPS, L'ю', L'Ю', WCH_NONE, L'ѫ'    , L'Ѫ'    }, {VK__none_, 0,  '.',  '>'},
	{VK_OEM_2     , SGCAPS,  '.',  ',', WCH_NONE,  ':'    ,  ';'    }, {VK__none_, 0,  '/',  '?'},
	{VK_OEM_102   , SGCAPS, '\\',  '/', WCH_FS  , L'ꙉ'    , L'Ꙉ'    }, {VK__none_, 0, '\\',  '|'},
	{0}
};

// Put this last so that VkKeyScan interprets number characters
// as coming from the main section of the kbd (aVkToWch0 and aVkToWch5)
// before considering the numpad (aVkToWch1).
static ALLOC_SECTION_LDATA VK_TO_WCHARS4 aVkToWch5[] = {
//	              │    │  Shift  │ Control │ Kana│
//	              ╞════╪═════════╪═════════╪═════╡
	{VK_NUMPAD0, 0, '0', WCH_NONE, WCH_NONE, L'↔'},
	{VK_NUMPAD1, 0, '1', WCH_NONE, WCH_NONE, L'↙'},
	{VK_NUMPAD2, 0, '2', WCH_NONE, WCH_NONE, L'↓'},
	{VK_NUMPAD3, 0, '3', WCH_NONE, WCH_NONE, L'↘'},
	{VK_NUMPAD4, 0, '4', WCH_NONE, WCH_NONE, L'←'},
	{VK_NUMPAD5, 0, '5', WCH_NONE, WCH_NONE, L'↕'},
	{VK_NUMPAD6, 0, '6', WCH_NONE, WCH_NONE, L'→'},
	{VK_NUMPAD7, 0, '7', WCH_NONE, WCH_NONE, L'↖'},
	{VK_NUMPAD8, 0, '8', WCH_NONE, WCH_NONE, L'↑'},
	{VK_NUMPAD9, 0, '9', WCH_NONE, WCH_NONE, L'↗'},
	{0}
};

static ALLOC_SECTION_LDATA VK_TO_WCHAR_TABLE aVkToWcharTable[] = {
	VK_TO_WCHAR_TABLE_ENTRY(aVkToWch0),
	VK_TO_WCHAR_TABLE_ENTRY(aVkToWch1),
	VK_TO_WCHAR_TABLE_ENTRY(aVkToWch2),
	VK_TO_WCHAR_TABLE_ENTRY(aVkToWch3),
	VK_TO_WCHAR_TABLE_ENTRY(aVkToWch4),
	VK_TO_WCHAR_TABLE_ENTRY(aVkToWch5),
	NULL
};

/***************************************************************************\
* aKeyNames[], aKeyNamesExt[]  - Virtual Scancode to Key Name tables
*
* Table attributes: Ordered Scan (by scancode), null-terminated
*
* Only the names of Extended, NumPad, Dead and Non-Printable keys are here.
* (Keys producing printable characters are named by that character)
\***************************************************************************/
static ALLOC_SECTION_LDATA VSC_LPWSTR aKeyNames[] = {
	0x01, L"Esc",
	0x0e, L"Backspace",
	0x0f, L"Tab",
	0x1c, L"Enter",
	0x1d, L"Ctrl",
	0x2a, L"Shift",
	0x36, L"Правый Shift",
	0x37, L"Num *",
	0x38, L"Alt",
	0x39, L"Space",
	0x3a, L"Caps Lock",
	0x3b, L"F1",
	0x3c, L"F2",
	0x3d, L"F3",
	0x3e, L"F4",
	0x3f, L"F5",
	0x40, L"F6",
	0x41, L"F7",
	0x42, L"F8",
	0x43, L"F9",
	0x44, L"F10",
	0x45, L"Pause",
	0x46, L"Scroll Lock",
	0x47, L"Num 7",
	0x48, L"Num 8",
	0x49, L"Num 9",
	0x4a, L"Num −",
	0x4b, L"Num 4",
	0x4c, L"Num 5",
	0x4d, L"Num 6",
	0x4e, L"Num +",
	0x4f, L"Num 1",
	0x50, L"Num 2",
	0x51, L"Num 3",
	0x52, L"Num 0",
	0x53, L"Num Del",
	0x54, L"Sys Req",
	0x57, L"F11",
	0x58, L"F12",
	0x7c, L"F13",
	0x7d, L"F14",
	0x7e, L"F15",
	0x7f, L"F16",
	0x80, L"F17",
	0x81, L"F18",
	0x82, L"F19",
	0x83, L"F20",
	0x84, L"F21",
	0x85, L"F22",
	0x86, L"F23",
	0x87, L"F24",
	0
};

static ALLOC_SECTION_LDATA VSC_LPWSTR aKeyNamesExt[] = {
	0x1c, L"Num Enter",
	0x1d, L"Правый Ctrl",
	0x35, L"Num /",
	0x37, L"Print Screen",
	0x38, L"Kana",
	0x45, L"Num Lock",
	0x46, L"Break",
	0x47, L"Home",
	0x48, L"Up",
	0x49, L"Page Up",
	0x4b, L"Left",
	0x4d, L"Right",
	0x4f, L"End",
	0x50, L"Down",
	0x51, L"Page Down",
	0x52, L"Insert",
	0x53, L"Delete",
	0x54, L"<00>",
	0x56, L"Help",
	0x5b, L"Лѣвый Windows",
	0x5c, L"Правый Windows",
	0x5d, L"Application",
	0
};

static ALLOC_SECTION_LDATA DEADKEY_LPWSTR aKeyNamesDead[] = {
	L"°Sup",
	L"ᵢSub",
	NULL
};

static ALLOC_SECTION_LDATA DEADKEY aDeadKey[] = {
	DEADTRANS(L' ', L'°', L'°', 0),
	DEADTRANS(L'0', L'°', L'⁰', 0),
	DEADTRANS(L'1', L'°', L'¹', 0),
	DEADTRANS(L'2', L'°', L'²', 0),
	DEADTRANS(L'3', L'°', L'³', 0),
	DEADTRANS(L'4', L'°', L'⁴', 0),
	DEADTRANS(L'5', L'°', L'⁵', 0),
	DEADTRANS(L'6', L'°', L'⁶', 0),
	DEADTRANS(L'7', L'°', L'⁷', 0),
	DEADTRANS(L'8', L'°', L'⁸', 0),
	DEADTRANS(L'9', L'°', L'⁹', 0),
	DEADTRANS(L'n', L'°', L'ⁿ', 0),

	DEADTRANS(L' ', L'ᵢ', L'ᵢ', 0),
	DEADTRANS(L'(', L'ᵢ', L'₍', 0),
	DEADTRANS(L')', L'ᵢ', L'₎', 0),
	DEADTRANS(L'+', L'ᵢ', L'₊', 0),
	DEADTRANS(L'-', L'ᵢ', L'₋', 0),
	DEADTRANS(L'1', L'ᵢ', L'₁', 0),
	DEADTRANS(L'2', L'ᵢ', L'₂', 0),
	DEADTRANS(L'3', L'ᵢ', L'₃', 0),
	DEADTRANS(L'4', L'ᵢ', L'₄', 0),
	DEADTRANS(L'5', L'ᵢ', L'₅', 0),
	DEADTRANS(L'6', L'ᵢ', L'₆', 0),
	DEADTRANS(L'7', L'ᵢ', L'₇', 0),
	DEADTRANS(L'8', L'ᵢ', L'₈', 0),
	DEADTRANS(L'9', L'ᵢ', L'₉', 0),
	DEADTRANS(L'=', L'ᵢ', L'₌', 0),
	DEADTRANS(L'a', L'ᵢ', L'ₐ', 0),
	DEADTRANS(L'e', L'ᵢ', L'ₑ', 0),
	DEADTRANS(L'h', L'ᵢ', L'ₕ', 0),
	DEADTRANS(L'i', L'ᵢ', L'ᵢ', 0),
	DEADTRANS(L'j', L'ᵢ', L'ⱼ', 0),
	DEADTRANS(L'k', L'ᵢ', L'ₖ', 0),
	DEADTRANS(L'l', L'ᵢ', L'ₗ', 0),
	DEADTRANS(L'm', L'ᵢ', L'ₘ', 0),
	DEADTRANS(L'n', L'ᵢ', L'ₙ', 0),
	DEADTRANS(L'o', L'ᵢ', L'ₒ', 0),
	DEADTRANS(L'p', L'ᵢ', L'ₚ', 0),
	DEADTRANS(L'r', L'ᵢ', L'ᵣ', 0),
	DEADTRANS(L's', L'ᵢ', L'ₛ', 0),
	DEADTRANS(L't', L'ᵢ', L'ₜ', 0),
	DEADTRANS(L'u', L'ᵢ', L'ᵤ', 0),
	DEADTRANS(L'v', L'ᵢ', L'ᵥ', 0),
	DEADTRANS(L'x', L'ᵢ', L'ₓ', 0),
	DEADTRANS(L'ә', L'ᵢ', L'ₔ', 0),

	0
};

static ALLOC_SECTION_LDATA KBDTABLES KbdTables = {
	/*
	 * Modifier keys
	 */
	&CharModifiers,

	/*
	 * Characters tables
	 */
	aVkToWcharTable,

	/*
	 * Diacritics
	 */
	aDeadKey,

	/*
	 * Names of Keys
	 */
	aKeyNames,
	aKeyNamesExt,
	aKeyNamesDead,

	/*
	 * Scan codes to Virtual Keys
	 */
	ausVK, sizeof ausVK / sizeof ausVK[0],
	aE0VscToVk,
	aE1VscToVk,

	/*
	 * Locale-specific special processing
	 */
	MAKELONG(0, KBD_VERSION),
};

// user32!OpenKeyboardLayoutFileWorker
// user32!OpenKeyboardLayoutFile
// user32!CommonCreateWindowStation
// user32!CreateWindowStationW
// winlogon!CreatePrimaryTerminal
// winlogon!CSession::CreatePrimaryTerminal
// winlogon!WinMain

PKBDTABLES KbdLayerDescriptor(VOID) // @1
{
	return &KbdTables;
}
