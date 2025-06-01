#define KBD_TYPE KEYBOARD_TYPE_GENERIC_101

#include <Windows.h>
#include <kbd.h>

#pragma data_seg(".data")
#define ALLOC_SECTION_LDATA __declspec(allocate(".data"))

#define VK_POWER 0x5E

#define WCH_ESC '\x1B'
#define WCH_FS  '\x1C'
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
	0x10, X10 | KBDEXT,  // Speedracer: Previous Track
	0x19, X19 | KBDEXT,  // Speedracer: Next Track
	0x1C, X1C | KBDEXT,  // Numpad Enter
	0x1D, X1D | KBDEXT,  // RControl
	0x20, X20 | KBDEXT,  // Speedracer: Volume Mute
	0x21, X21 | KBDEXT,  // Speedracer: Launch App 2
	0x22, X22 | KBDEXT,  // Speedracer: Media Play/Pause
	0x24, X24 | KBDEXT,  // Speedracer: Media Stop
	0x2E, X2E | KBDEXT,  // Speedracer: Volume Down
	0x30, X30 | KBDEXT,  // Speedracer: Volume Up
	0x32, X32 | KBDEXT,  // Speedracer: Browser Home
	0x35, X35 | KBDEXT,  // Numpad Divide
	0x37, X37 | KBDEXT,  // Snapshot
	0x38, X38 | KBDEXT,  // RMenu
	0x46, X46 | KBDEXT,  // Break (Ctrl + Pause)
	0x47, X47 | KBDEXT,  // Home
	0x48, X48 | KBDEXT,  // Up
	0x49, X49 | KBDEXT,  // Prior
	0x4B, X4B | KBDEXT,  // Left
	0x4D, X4D | KBDEXT,  // Right
	0x4F, X4F | KBDEXT,  // End
	0x50, X50 | KBDEXT,  // Down
	0x51, X51 | KBDEXT,  // Next
	0x52, X52 | KBDEXT,  // Insert
	0x53, X53 | KBDEXT,  // Delete
	0x5B, X5B | KBDEXT,  // Left Win
	0x5C, X5C | KBDEXT,  // Right Win
	0x5D, X5D | KBDEXT,  // Application
	0x5F, X5F | KBDEXT,  // Speedracer: Sleep
	0x65, X65 | KBDEXT,  // Speedracer: Browser Search
	0x66, X66 | KBDEXT,  // Speedracer: Browser Favorites
	0x67, X67 | KBDEXT,  // Speedracer: Browser Refresh
	0x68, X68 | KBDEXT,  // Speedracer: Browser Stop
	0x69, X69 | KBDEXT,  // Speedracer: Browser Forward
	0x6A, X6A | KBDEXT,  // Speedracer: Browser Back
	0x6B, X6B | KBDEXT,  // Speedracer: Launch App 1
	0x6C, X6C | KBDEXT,  // Speedracer: Launch Mail
	0x6D, X6D | KBDEXT,  // Speedracer: Launch Media Selector
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
//	VK_KANA,        KBDKANA,
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
	7,
	{
	//  /*      */ Modification# Shift         // Keys Pressed
	//  /* ════ */ ════════════, ════════════, /* ════════════════════ */
		/*      */ 0           , 1           , /*        	           */
		/*      */ 2           , SHFT_INVALID, /* Control	           */
		/*      */ SHFT_INVALID, SHFT_INVALID, /* Alt    	           */
		/*      */ 3           , 4           , /* AltGr  	           */
	//	/* Kana */ 5           , 6           , /*        	           */
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
* aVkToWchN[]  - Virtual Key to WCHAR translation for N shift state
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
static ALLOC_SECTION_LDATA VK_TO_WCHARS2 aVkToWch2[] = {
//	                 |   |Shift|
//	                 |===|=====|
	{VK_TAB     , 0, '\t', '\t'},
	{VK_ADD     , 0,  '+',  '+'},
	{VK_SUBTRACT, 0,  '-',  '-'},
	{0}
};

static ALLOC_SECTION_LDATA VK_TO_WCHARS3 aVkToWch3[] = {
//	             |        | Shift  | Control |
//	             |========|========|=========|
	{VK_CANCEL, 0,  '\3'  ,  '\3'  ,  '\3'   },
	{VK_BACK  , 0,  '\b'  ,  '\b'  , WCH_DEL },
	{VK_RETURN, 0,  '\r'  ,  '\r'  ,  '\n'   },
	{VK_ESCAPE, 0, WCH_ESC, WCH_ESC, WCH_ESC },
	{0}
};

static ALLOC_SECTION_LDATA VK_TO_WCHARS5 aVkToWch5[] = {
//	                      │     │Shift│ Control │  AltGr  │S + AltGr│	                 │         │  Shift  │ Control │  AltGr  │S+AltGr│
//	                      ╞═════╪═════╪═════════╪═════════╪═════════╡	                 ╞═════════╪═════════╪═════════╪═════════╪═══════╡
	{'1'          , SGCAPS, '1' , '!' , WCH_NONE, WCH_NONE, WCH_NONE},	{'1'          , 0, L'¹'},
	{'2'          , SGCAPS, '2' , '@' , WCH_NONE, L'ě'    , L'½'    },	{'2'          , 0, L'²'    , L'½'    },
	{'3'          , SGCAPS, '3' , '#' , WCH_NONE, L'§'    , L'⅓'    },	{'3'          , 0, L'³'    , L'⅓'    },
	{'4'          , SGCAPS, '4' , '$' , WCH_NONE, L'₽'    , L'€'    },	{'4'          , 0, L'⁴'    , L'¼'    },
	{'5'          , SGCAPS, '5' , '%' , WCH_NONE, L'°'    , L'⁴'    },	{'5'          , 0, L'⁵'    , L'¾'    },
	{'6'          , SGCAPS, '6' , '^' , WCH_NONE, L'ı'    ,  'I'    },	{'6'          , 0, L'⁶'    , L'⅚'    },
	{'7'          , SGCAPS, '7' , '&' , WCH_NONE, L'∞'    , L'³'    },	{'7'          , 0, L'⁷'    , L'⅞'    },
	{'8'          , SGCAPS, '8' , '*' , WCH_NONE, L'×'    , L'∏'    },	{'8'          , 0, L'⁸'    , L'⅛'    },
	{'9'          , SGCAPS, '9' , '(' , WCH_NONE, L'≤'    , L'∀'   },	{'9'          , 0, L'⁹'    , L'⅜'    },
	{'0'          , SGCAPS, '0' , ')' , WCH_NONE, L'≥'    , L'҂'    },	{'0'          , 0, L'⁰'    , L'⅝'    },
	{VK_OEM_MINUS , SGCAPS, '-' , '_' , WCH_NONE, L'±'    , L'—'    },	{VK_OEM_MINUS , 0, WCH_NONE, L'⅞'    },
	{VK_OEM_PLUS  , 0     , '=' , '+' , WCH_NONE, WCH_DEAD, WCH_DEAD},	{VK__none_    , 0, WCH_NONE, WCH_NONE, WCH_NONE, L'´'    , L'ˇ'  },
	{'Q'          , SGCAPS, 'q' , 'Q' , WCH_NONE, L'ä'    , L'Ä'    },	{'Q'          , 0, L'ο'    , L'Ο'    },
	{'W'          , SGCAPS, 'w' , 'W' , WCH_NONE, L'ś'    , L'Ś'    },	{'W'          , 0, L'ψ'    , L'Ψ'    },
	{'E'          , SGCAPS, 'e' , 'E' , WCH_NONE, L'ě'    , L'Ě'    },	{'E'          , 0, L'ε'    , L'Ε'    },
	{'R'          , SGCAPS, 'r' , 'R' , WCH_NONE, L'ř'    , L'Ř'    },	{'R'          , 0, L'ρ'    , L'Ρ'    },
	{'T'          , SGCAPS, 't' , 'T' , WCH_NONE, L'ť'    , L'Ť'    },	{'T'          , 0, L'θ'    , L'Θ'    },
	{'Y'          , SGCAPS, 'y' , 'Y' , WCH_NONE, L'ý'    , L'Ý'    },	{'Y'          , 0, L'υ'    , L'Υ'    },
	{'U'          , CAPLOK, 'u' , 'U' , WCH_NONE, L'ů'    , L'Ů'    },
	{'I'          , SGCAPS, 'i' , 'I' , WCH_NONE, L'í'    , L'Í'    },	{'I'          , 0, L'ι'    , L'Ι'    },
	{'O'          , SGCAPS, 'o' , 'O' , WCH_NONE, L'ô'    , L'Ô'    },	{'O'          , 0, L'ω'    , L'Ω'    },
	{'P'          , SGCAPS, 'p' , 'P' , WCH_NONE, L'ś'    , L'Ś'    },	{'P'          , 0, L'π'    , L'Π'    },
	{VK_OEM_4     , SGCAPS, '[' , '{' , WCH_NONE, L'ź'    , L'Ź'    },	{VK_OEM_4     , 0, WCH_NONE, L'┬'    },
	{VK_OEM_6     , SGCAPS, ']' , '}' , '\x001b', L'ć'    , L'Ć'    },	{VK_OEM_6     , 0, WCH_NONE, L'┤'    },
	{'A'          , SGCAPS, 'a' , 'A' , WCH_NONE, L'á'    , L'Á'    },	{'A'          , 0, L'α'    , L'Α'    },
	{'S'          , SGCAPS, 's' , 'S' , WCH_NONE, L'š'    , L'Š'    },	{'S'          , 0, L'σ'    , L'Σ'    },
	{'D'          , SGCAPS, 'd' , 'D' , WCH_NONE, L'ď'    , L'Ď'    },	{'D'          , 0, L'δ'    , L'Δ'    },
	{'F'          , SGCAPS, 'f' , 'F' , WCH_NONE, L'ę'    , L'Ę'    },	{'F'          , 0, L'φ'    , L'Φ'    },
	{'G'          , SGCAPS, 'g' , 'G' , WCH_NONE, L'ț'    , L'Ț'    },	{'G'          , 0, L'γ'    , L'Γ'    },
	{'H'          , SGCAPS, 'h' , 'H' , WCH_NONE, L'ą'    , L'Ą'    },	{'H'          , 0, L'η'    , L'Η'    },
	{'J'          , SGCAPS, 'j' , 'J' , WCH_NONE, L'ó'    , L'Ó'    },	{'J'          , 0, L'¢'    , L'£'    },
	{'K'          , SGCAPS, 'k' , 'K' , WCH_NONE, L'ł'    , L'Ł'    },	{'K'          , 0, L'κ'    , L'Κ'    },
	{'L'          , SGCAPS, 'l' , 'L' , WCH_NONE, L'ľ'    , L'Ľ'    },	{'L'          , 0, L'λ'    , L'Λ'    },
	{VK_OEM_1     , SGCAPS, ';' , ':' , '\x001d', L'ż'    , L'Ż'    },	{VK_OEM_1     , 0, WCH_NONE, L'┴'    },
	{VK_OEM_7     , SGCAPS, '\'', '\"', WCH_NONE, L'é'    , L'É'    },	{VK_OEM_7     , 0, WCH_NONE, L'├'    },
	{VK_OEM_3     , 0     , '`' , '~' , WCH_NONE, L'ą'    , WCH_DEAD},	{VK__none_    , 0, WCH_NONE, WCH_NONE, WCH_NONE, WCH_NONE, L'°'  },
	{VK_OEM_5     , SGCAPS, '\\', '|' , '\x001c', WCH_DEAD, WCH_DEAD},	{VK__none_    , 0, WCH_NONE, WCH_NONE, WCH_NONE, L'¨'    ,  '^'  },
	{'Z'          , SGCAPS, 'z' , 'Z' , WCH_NONE, L'ž'    , L'Ž'    },	{'Z'          , 0, L'ζ'    , L'Ζ'    },
	{'X'          , SGCAPS, 'x' , 'X' , WCH_NONE, L'ć'    , L'Ć'    },	{'X'          , 0, L'ξ'    , L'Ξ'    },
	{'C'          , SGCAPS, 'c' , 'C' , WCH_NONE, L'č'    , L'Č'    },	{'C'          , 0, L'ς'    , L'Σ'    },
	{'V'          , SGCAPS, 'v' , 'V' , WCH_NONE, L'ș'    , L'Ș'    },	{'V'          , 0, L'¥'    , L'¥'    },
	{'B'          , SGCAPS, 'b' , 'B' , WCH_NONE, L'ß'    , L'ẞ'    },	{'B'          , 0, L'β'    , L'Β'    },
	{'N'          , SGCAPS, 'n' , 'N' , WCH_NONE, L'ň'    , L'Ň'    },	{'N'          , 0, L'ν'    , L'Ν'    },
	{'M'          , SGCAPS, 'm' , 'M' , WCH_NONE, L'ń'    , L'Ń'    },	{'M'          , 0, L'μ'    , L'Μ'    },
	{VK_OEM_COMMA , 0     , ',' , '<' , WCH_NONE, L'μ'    , L'Μ'    },
	{VK_OEM_PERIOD, SGCAPS, '.' , '>' , WCH_NONE, L'ú'    , L'Ú'    },	{VK_OEM_PERIOD, 0, L'…'    , L'─'    },
	{VK_OEM_2     , SGCAPS, '/' , '?' , WCH_NONE, L'÷'    , L'∕'    },	{VK_OEM_2     , 0, WCH_NONE, L'│'    },
	{VK_SPACE     , 0     ,  ' ' ,  ' ' ,  ' '    , L' '  , L' '    },
	{VK_OEM_102   , 0     , '\\', '|' , WCH_NONE, L'ß'    , WCH_DEAD},	{VK__none_    , 0, WCH_NONE, WCH_NONE, WCH_NONE, WCH_NONE, L'˝'  },
	{0}
};

// Put this last so that VkKeyScan interprets number characters
// as coming from the main section of the kbd (aVkToWch1 and aVkToWch5)
// before considering the numpad (aVkToWch4).
static ALLOC_SECTION_LDATA VK_TO_WCHARS4 aVkToWch4[] = {
//	              │    │  Shift  │ Control │  AltGr  │
//	              ╞════╪═════════╪═════════╪═════════╡
	{VK_NUMPAD0 , 0, '0', WCH_NONE, WCH_NONE, WCH_NONE},
	{VK_NUMPAD1 , 0, '1', WCH_NONE, WCH_NONE, L'└'    },
	{VK_NUMPAD2 , 0, '2', WCH_NONE, WCH_NONE, L'↓'    },
	{VK_NUMPAD3 , 0, '3', WCH_NONE, WCH_NONE, L'┘'    },
	{VK_NUMPAD4 , 0, '4', WCH_NONE, WCH_NONE, L'←'    },
	{VK_NUMPAD5 , 0, '5', WCH_NONE, WCH_NONE, L'┼'    },
	{VK_NUMPAD6 , 0, '6', WCH_NONE, WCH_NONE, L'→'    },
	{VK_NUMPAD7 , 0, '7', WCH_NONE, WCH_NONE, L'┌'    },
	{VK_NUMPAD8 , 0, '8', WCH_NONE, WCH_NONE, L'↑'    },
	{VK_NUMPAD9 , 0, '9', WCH_NONE, WCH_NONE, L'┐'    },
	{VK_MULTIPLY, 0, '*', '*'     , WCH_NONE, L'↔'    },
	{VK_DIVIDE  , 0, '/', '/'     , WCH_NONE, L'↕'    },
	{VK_DECIMAL , 0, '.', ','     , WCH_NONE,  '.'    },
	{0}
};

static ALLOC_SECTION_LDATA VK_TO_WCHAR_TABLE aVkToWcharTable[] = {
	(PVK_TO_WCHARS1)aVkToWch3, 3, sizeof aVkToWch3[0],
	(PVK_TO_WCHARS1)aVkToWch5, 5, sizeof aVkToWch5[0],
	(PVK_TO_WCHARS1)aVkToWch2, 2, sizeof aVkToWch2[0],
	(PVK_TO_WCHARS1)aVkToWch4, 4, sizeof aVkToWch4[0],
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
	0x37, L"Prnt Scrn",
	0x38, L"Правый Alt",
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
	L"´Čárka",
	L"ˇHáček",
	L"°Kroužek",
	L"¨DIAERESIS",
	L"^CIRCUMFLEX ACCENT",
	L"˝DOUBLE ACUTE ACCENT",
	NULL
};

static ALLOC_SECTION_LDATA DEADKEY aDeadKey[] = {
	DEADTRANS(L' ', L'´', L'´', 0),
	DEADTRANS(L'A', L'´', L'Á', 0),
	DEADTRANS(L'C', L'´', L'Ć', 0),
	DEADTRANS(L'E', L'´', L'É', 0),
	DEADTRANS(L'G', L'´', L'Ǵ', 0),
	DEADTRANS(L'I', L'´', L'Í', 0),
	DEADTRANS(L'L', L'´', L'Ĺ', 0),
	DEADTRANS(L'N', L'´', L'Ń', 0),
	DEADTRANS(L'O', L'´', L'Ó', 0),
	DEADTRANS(L'R', L'´', L'Ŕ', 0),
	DEADTRANS(L'S', L'´', L'Ś', 0),
	DEADTRANS(L'U', L'´', L'Ú', 0),
	DEADTRANS(L'Y', L'´', L'Ý', 0),
	DEADTRANS(L'Z', L'´', L'Ź', 0),
	DEADTRANS(L'a', L'´', L'á', 0),
	DEADTRANS(L'c', L'´', L'ć', 0),
	DEADTRANS(L'e', L'´', L'é', 0),
	DEADTRANS(L'g', L'´', L'ǵ', 0),
	DEADTRANS(L'i', L'´', L'í', 0),
	DEADTRANS(L'l', L'´', L'ĺ', 0),
	DEADTRANS(L'n', L'´', L'ń', 0),
	DEADTRANS(L'o', L'´', L'ó', 0),
	DEADTRANS(L'r', L'´', L'ŕ', 0),
	DEADTRANS(L's', L'´', L'ś', 0),
	DEADTRANS(L'u', L'´', L'ú', 0),
	DEADTRANS(L'y', L'´', L'ý', 0),
	DEADTRANS(L'z', L'´', L'ź', 0),

	DEADTRANS(L' ', L'ˇ', L'ˇ', 0),
	DEADTRANS(L'A', L'ˇ', L'Ǎ', 0),
	DEADTRANS(L'C', L'ˇ', L'Č', 0),
	DEADTRANS(L'D', L'ˇ', L'Ď', 0),
	DEADTRANS(L'E', L'ˇ', L'Ě', 0),
	DEADTRANS(L'G', L'ˇ', L'Ȟ', 0),
	DEADTRANS(L'H', L'ˇ', L'ȟ', 0),
	DEADTRANS(L'I', L'ˇ', L'Ǐ', 0),
	DEADTRANS(L'K', L'ˇ', L'Ǩ', 0),
	DEADTRANS(L'L', L'ˇ', L'Ľ', 0),
	DEADTRANS(L'N', L'ˇ', L'Ň', 0),
	DEADTRANS(L'R', L'ˇ', L'Ř', 0),
	DEADTRANS(L'S', L'ˇ', L'Š', 0),
	DEADTRANS(L'T', L'ˇ', L'Ť', 0),
	DEADTRANS(L'O', L'ˇ', L'Ǒ', 0),
	DEADTRANS(L'U', L'ˇ', L'Ǔ', 0),
	DEADTRANS(L'Z', L'ˇ', L'Ž', 0),
	DEADTRANS(L'З', L'ˇ', L'Ǯ', 0),
	DEADTRANS(L'Џ', L'ˇ', L'Ǆ', 0),
	DEADTRANS(L'a', L'ˇ', L'ǎ', 0),
	DEADTRANS(L'c', L'ˇ', L'č', 0),
	DEADTRANS(L'd', L'ˇ', L'ď', 0),
	DEADTRANS(L'e', L'ˇ', L'ě', 0),
	DEADTRANS(L'g', L'ˇ', L'ǧ', 0),
	DEADTRANS(L'h', L'ˇ', L'ȟ', 0),
	DEADTRANS(L'i', L'ˇ', L'ǐ', 0),
	DEADTRANS(L'j', L'ˇ', L'ǰ', 0),
	DEADTRANS(L'k', L'ˇ', L'ǩ', 0),
	DEADTRANS(L'l', L'ˇ', L'ľ', 0),
	DEADTRANS(L'n', L'ˇ', L'ň', 0),
	DEADTRANS(L'r', L'ˇ', L'ř', 0),
	DEADTRANS(L's', L'ˇ', L'š', 0),
	DEADTRANS(L't', L'ˇ', L'ť', 0),
	DEADTRANS(L'o', L'ˇ', L'ǒ', 0),
	DEADTRANS(L'u', L'ˇ', L'ǔ', 0),
	DEADTRANS(L'z', L'ˇ', L'ž', 0),
	DEADTRANS(L'џ', L'ˇ', L'ǅ', 0),
	DEADTRANS(L'з', L'ˇ', L'ǯ', 0),

	DEADTRANS(L' ', L'°', L'°', 0),
	DEADTRANS(L'A', L'°', L'Å', 0),
	DEADTRANS(L'U', L'°', L'Ů', 0),
	DEADTRANS(L'a', L'°', L'å', 0),
	DEADTRANS(L'u', L'°', L'ů', 0),

	DEADTRANS(L' ', L'¨', L'¨', 0),
	DEADTRANS(L'A', L'¨', L'Ä', 0),
	DEADTRANS(L'E', L'¨', L'Ë', 0),
	DEADTRANS(L'I', L'¨', L'Ï', 0),
	DEADTRANS(L'O', L'¨', L'Ö', 0),
	DEADTRANS(L'U', L'¨', L'Ü', 0),
	DEADTRANS(L'Y', L'¨', L'Ÿ', 0),
	DEADTRANS(L'a', L'¨', L'ä', 0),
	DEADTRANS(L'e', L'¨', L'ë', 0),
	DEADTRANS(L'i', L'¨', L'ï', 0),
	DEADTRANS(L'o', L'¨', L'ö', 0),
	DEADTRANS(L'u', L'¨', L'ü', 0),
	DEADTRANS(L'y', L'¨', L'ÿ', 0),

	DEADTRANS(L' ', L'^', L'^', 0),
	DEADTRANS(L'A', L'^', L'Â', 0),
	DEADTRANS(L'E', L'^', L'Ê', 0),
	DEADTRANS(L'I', L'^', L'Î', 0),
	DEADTRANS(L'O', L'^', L'Ô', 0),
	DEADTRANS(L'U', L'^', L'Û', 0),
	DEADTRANS(L'a', L'^', L'â', 0),
	DEADTRANS(L'e', L'^', L'ê', 0),
	DEADTRANS(L'i', L'^', L'î', 0),
	DEADTRANS(L'o', L'^', L'ô', 0),
	DEADTRANS(L'u', L'^', L'û', 0),

	DEADTRANS(L' ', L'°', L'°', 0),
	DEADTRANS(L'A', L'°', L'Å', 0),
	DEADTRANS(L'a', L'°', L'å', 0),
	DEADTRANS(L'U', L'°', L'Ů', 0),
	DEADTRANS(L'u', L'°', L'ů', 0),
	DEADTRANS(L'w', L'°', L'ẘ', 0),
	DEADTRANS(L'y', L'°', L'ẙ', 0),

	DEADTRANS(L' ', L'˝', L'˝', 0),
	DEADTRANS(L'O', L'˝', L'Ő', 0),
	DEADTRANS(L'o', L'˝', L'ő', 0),
	DEADTRANS(L'U', L'˝', L'Ű', 0),
	DEADTRANS(L'u', L'˝', L'ű', 0),
	DEADTRANS(L'У', L'˝', L'Ӳ', 0),
	DEADTRANS(L'у', L'˝', L'ӳ', 0),

	L'\0',
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
	MAKELONG(KLLF_ALTGR, KBD_VERSION),
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

PKBDNLSTABLES KbdNlsLayerDescriptor(VOID) // @2
{
	return nullptr;
}

typedef struct _CLIENTKEYBOARDTYPE {
	ULONG Type;
	ULONG SubType;
	ULONG FunctionKey;
} CLIENTKEYBOARDTYPE, *KBD_LONG_POINTER PCLIENTKEYBOARDTYPE;

_Success_(return) BOOL KbdRealLayoutFileNT4(_Out_z_cap_c_(MAX_PATH) LPWSTR RealLayoutFile) // @3
{
	return false;
}

_Success_(return) BOOL KbdRealLayoutFile(HKL hKL, _Out_z_cap_c_(MAX_PATH) LPWSTR RealLayoutFile, _In_ PCLIENTKEYBOARDTYPE ClientKbdType, LPVOID) // @5
{
	return false;
}

_Success_(return) BOOL KbdMultiLayout(_Out_ PKBDTABLE_MULTI Multi) // @6
{
	return false;
}

#ifdef _DEBUG
EXTERN_C BOOL APIENTRY _DllMainCRTStartup(HMODULE hLib, DWORD dwReason, PCONTEXT)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH: __debugbreak(); DisableThreadLibraryCalls(hLib); break;
	case DLL_PROCESS_DETACH: __debugbreak(); break;
	case DLL_THREAD_ATTACH: break;
	case DLL_THREAD_DETACH: break;
	}
	return true;
}
#endif
