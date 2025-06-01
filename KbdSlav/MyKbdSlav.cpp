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
	VK_KANA,        KBDKANA,
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
		/*      */ 3           , 4           , /* AltGr  	           */
		/* Kana */ 5           , 6           , /*        	           */
		/* Kana */ SHFT_INVALID, SHFT_INVALID, /* Control	           */
		/* Kana */ SHFT_INVALID, SHFT_INVALID, /* Alt    	           */
		/* Kana */ SHFT_INVALID, SHFT_INVALID, /* AltGr  	           */
		/*      */ SHFT_INVALID, SHFT_INVALID, /*        	      Roya */
		/*      */ SHFT_INVALID, SHFT_INVALID, /* Control	      Roya */
		/*      */ SHFT_INVALID, SHFT_INVALID, /* Alt    	      Roya */
		/*      */ SHFT_INVALID, SHFT_INVALID, /* AltGr  	      Roya */
		/* Kana */ SHFT_INVALID, SHFT_INVALID, /*        	      Roya */
		/* Kana */ SHFT_INVALID, SHFT_INVALID, /* Control	      Roya */
		/* Kana */ SHFT_INVALID, SHFT_INVALID, /* Alt    	      Roya */
		/* Kana */ SHFT_INVALID, SHFT_INVALID, /* AltGr  	      Roya */
		/*      */ SHFT_INVALID, SHFT_INVALID, /*        	 Loya      */
		/*      */ SHFT_INVALID, SHFT_INVALID, /* Control	 Loya      */
		/*      */ SHFT_INVALID, SHFT_INVALID, /* Alt    	 Loya      */
		/*      */ SHFT_INVALID, SHFT_INVALID, /* AltGr  	 Loya      */
		/* Kana */ SHFT_INVALID, SHFT_INVALID, /*        	 Loya      */
		/* Kana */ SHFT_INVALID, SHFT_INVALID, /* Control	 Loya      */
		/* Kana */ SHFT_INVALID, SHFT_INVALID, /* Alt    	 Loya      */
		/* Kana */ SHFT_INVALID, SHFT_INVALID, /* AltGr  	 Loya      */
		/*      */ SHFT_INVALID, SHFT_INVALID, /*        	 Loya Roya */
		/*      */ SHFT_INVALID, SHFT_INVALID, /* Control	 Loya Roya */
		/*      */ SHFT_INVALID, SHFT_INVALID, /* Alt    	 Loya Roya */
		/*      */ SHFT_INVALID, SHFT_INVALID, /* AltGr  	 Loya Roya */
		/* Kana */ SHFT_INVALID, SHFT_INVALID, /*        	 Loya Roya */
		/* Kana */ SHFT_INVALID, SHFT_INVALID, /* Control	 Loya Roya */
		/* Kana */ SHFT_INVALID, SHFT_INVALID, /* Alt    	 Loya Roya */
		/* Kana */ SHFT_INVALID, SHFT_INVALID, /* AltGr  	 Loya Roya */
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
//	             |        | Shift  | Control |
//	             |========|========|=========|
	{VK_CANCEL, 0,  '\3'  ,  '\3'  ,  '\3'   },
	{VK_BACK  , 0,  '\b'  ,  '\b'  , WCH_DEL },
	{VK_RETURN, 0,  '\r'  ,  '\r'  ,  '\n'   },
	{VK_ESCAPE, 0, WCH_ESC, WCH_ESC, WCH_ESC },
	{0}
};

static ALLOC_SECTION_LDATA VK_TO_WCHARS4 aVkToWch2[] = {
//	            │    │Shift│Control│AltGr│
//	            ╞════╪═════╪═══════╪═════╡
	{VK_SPACE, 0, ' ', ' ' ,  ' '  , L' '},
	{0}
};

static ALLOC_SECTION_LDATA VK_TO_WCHARS5 aVkToWch3[] = {
//	                    │     │Shift│ Control │AltGr│ ↑AltGr↑ │
//	                    ╞═════╪═════╪═════════╪═════╪═════════╡
	{VK_DIVIDE  , SGCAPS, L'÷', '/' , WCH_NONE, L'∕', L'⁄'    }, {VK__none_, 0, '/', '/'},
	{VK_MULTIPLY, SGCAPS, L'×', '*' , WCH_NONE, L'⋅', L'⁢'     }, {VK__none_, 0, '*', '*'},
	{VK_SUBTRACT, SGCAPS, L'−', '-' , WCH_NONE, L'–', L'—'    }, {VK__none_, 0, '-', '-'},
	{VK_ADD     , 0     ,  '+', '+' , WCH_NONE, L'±', L'∓'    },
	{VK_DECIMAL , 0     ,  ',', '.' , WCH_NONE, L'…', L'‥'    },
	{0}
};

static ALLOC_SECTION_LDATA VK_TO_WCHARS7 aVkToWch4[] = {
//	                                              |     |Shift|   Ctrl  |AltGr|S + AltGr│     │Shift│
//	                                              |=====|=====|=========|=====|=========|=====╪═════╡
	{'1'          ,                        KANALOK,  '!',  '[', WCH_NONE, L'́', L'²'	, '1' ,  '!'},
	{'2'          ,                        KANALOK, L'«',  ']', WCH_NONE, L'̏', '\"'	, '2' ,  '@'},
	{'3'          ,                        KANALOK, L'»', L'№', WCH_NONE, L'̄', L'§'	, '3' ,  '#'},
	{'4'          ,                        KANALOK, L'₽', L'€', WCH_NONE,  '$', L'¥'	, '4' ,  '$'},
	{'5'          ,                        KANALOK,  '%', L'‰', WCH_NONE, L'°', L'½'	, '5' ,  '%'},
	{'6'          , CAPLOK               | KANALOK, L'ѣ', L'Ѣ', WCH_NONE, L'̂', L'⅔'	, '6' ,  '^'},
	{'7'          ,                        KANALOK,  '?',  '{', WCH_NONE,  '&', L'¾'	, '7' ,  '&'},
	{'8'          ,                        KANALOK, L'×',  '}', WCH_NONE, L'∞', L'∏'	, '8' ,  '*'},
	{'9'          ,                        KANALOK,  '(',  '<', WCH_NONE, L'҃', L'≤'	, '9' ,  '('},
	{'0'          ,                        KANALOK,  ')',  '>', WCH_NONE, L'҂', L'≥'	, '0' ,  ')'},
	{VK_OEM_MINUS ,                        KANALOK,  '-', L'−', WCH_NONE, L'­' , L'—'	, '-' ,  '_'},
	{VK_OEM_PLUS  ,                        KANALOK,  '=', L'≠', WCH_NONE, L'≈', L'≡'	, '=' ,  '+'},
	{'Q'          , CAPLOK | CAPLOKALTGR | KANALOK, L'й', L'Й', WCH_NONE, L'ј', L'Ј'	, 'q' ,  'Q'},
	{'W'          , CAPLOK | CAPLOKALTGR | KANALOK, L'ц', L'Ц', WCH_NONE, L'џ', L'Џ'	, 'w' ,  'W'},
	{'E'          , CAPLOK | CAPLOKALTGR | KANALOK, L'у', L'У', WCH_NONE, L'ѫ', L'Ѫ'	, 'e' ,  'E'},
	{'R'          , CAPLOK | CAPLOKALTGR | KANALOK, L'к', L'К', WCH_NONE, L'ќ', L'Ќ'	, 'r' ,  'R'},
	{'T'          , CAPLOK | CAPLOKALTGR | KANALOK, L'е', L'Е', WCH_NONE, L'ѣ', L'Ѣ'	, 't' ,  'T'},
	{'Y'          , CAPLOK | CAPLOKALTGR | KANALOK, L'н', L'Н', WCH_NONE, L'њ', L'Њ'	, 'y' ,  'Y'},
	{'U'          , CAPLOK | CAPLOKALTGR | KANALOK, L'г', L'Г', WCH_NONE, L'ѓ', L'Ѓ'	, 'u' ,  'U'},
	{'I'          , CAPLOK | CAPLOKALTGR | KANALOK, L'ш', L'Ш', WCH_NONE, L'ї', L'Ї'	, 'i' ,  'I'},
	{'O'          , CAPLOK | CAPLOKALTGR | KANALOK, L'щ', L'Щ', WCH_NONE, L'ѹ', L'Ѹ'	, 'o' ,  'O'},
	{'P'          , CAPLOK | CAPLOKALTGR | KANALOK, L'з', L'З', WCH_NONE, L'ꙁ', L'Ꙁ'	, 'p' ,  'P'},
	{VK_OEM_4     , CAPLOK | CAPLOKALTGR | KANALOK, L'х', L'Х', WCH_NONE, L'ꙗ', L'Ꙗ'	, '[' ,  '{'},
	{VK_OEM_6     , CAPLOK | CAPLOKALTGR | KANALOK, L'ъ', L'Ъ', WCH_NONE, L'ѩ', L'Ѩ'	, ']' ,  '}'},
	{'A'          , CAPLOK | CAPLOKALTGR | KANALOK, L'ф', L'Ф', WCH_NONE, L'ѳ', L'Ѳ'	, 'a' ,  'A'},
	{'S'          , CAPLOK | CAPLOKALTGR | KANALOK, L'ы', L'Ы', WCH_NONE, L'ѕ', L'Ѕ'	, 's' ,  'S'},
	{'D'          , CAPLOK | CAPLOKALTGR | KANALOK, L'в', L'В', WCH_NONE, L'ћ', L'Ћ'	, 'd' ,  'D'},
	{'F'          , CAPLOK | CAPLOKALTGR | KANALOK, L'а', L'А', WCH_NONE, L'ѧ', L'Ѧ'	, 'f' ,  'F'},
	{'G'          , CAPLOK | CAPLOKALTGR | KANALOK, L'п', L'П', WCH_NONE, L'ѱ', L'Ѱ'	, 'g' ,  'G'},
	{'H'          , CAPLOK | CAPLOKALTGR | KANALOK, L'р', L'Р', WCH_NONE, L'ѡ', L'Ѡ'	, 'h' ,  'H'},
	{'J'          , CAPLOK | CAPLOKALTGR | KANALOK, L'о', L'О', WCH_NONE, L'ꙋ', L'Ꙋ'	, 'j' ,  'J'},
	{'K'          , CAPLOK | CAPLOKALTGR | KANALOK, L'л', L'Л', WCH_NONE, L'љ', L'Љ'	, 'k' ,  'K'},
	{'L'          , CAPLOK | CAPLOKALTGR | KANALOK, L'д', L'Д', WCH_NONE, L'ѕ', L'Ѕ'	, 'l' ,  'L'},
	{VK_OEM_1     , CAPLOK | CAPLOKALTGR | KANALOK, L'ж', L'Ж', WCH_NONE, L'ꙃ', L'Ꙃ'	, ';' ,  ':'},
	{VK_OEM_7     , CAPLOK | CAPLOKALTGR | KANALOK, L'э', L'Э', WCH_NONE, L'є', L'Є'	,'\'' , '\"'},
	{VK_OEM_3     , CAPLOK               | KANALOK, L'ё', L'Ё', WCH_NONE, L'̀', L'³'	, '`' ,  '~'},
	{VK_OEM_5     , CAPLOK               | KANALOK, '\\',  '/', WCH_NONE, L'ѥ', L'Ѥ'	,'\\' ,  '|'},
	{'Z'          , CAPLOK | CAPLOKALTGR | KANALOK, L'я', L'Я', WCH_NONE, L'ђ', L'Ђ'	, 'z' ,  'Z'},
	{'X'          , CAPLOK | CAPLOKALTGR | KANALOK, L'ч', L'Ч', WCH_NONE, L'ѯ', L'Ѯ'	, 'x' ,  'X'},
	{'C'          , CAPLOK | CAPLOKALTGR | KANALOK, L'с', L'С', WCH_NONE, L'ҁ', L'Ҁ'	, 'c' ,  'C'},
	{'V'          , CAPLOK | CAPLOKALTGR | KANALOK, L'м', L'М', WCH_NONE, L'ў', L'Ў'	, 'v' ,  'V'},
	{'B'          , CAPLOK | CAPLOKALTGR | KANALOK, L'и', L'И', WCH_NONE, L'і', L'І'	, 'd' ,  'D'},
	{'N'          , CAPLOK | CAPLOKALTGR | KANALOK, L'т', L'Т', WCH_NONE, L'ѿ', L'Ѿ'	, 'n' ,  'N'},
	{'M'          , CAPLOK | CAPLOKALTGR | KANALOK, L'ь', L'Ь', WCH_NONE, L'ѵ', L'Ѵ'	, 'm' ,  'M'},
	{VK_OEM_COMMA , CAPLOK | CAPLOKALTGR | KANALOK, L'б', L'Б', WCH_NONE, L'ѫ', L'Ѫ'	, ',' ,  '<'},
	{VK_OEM_PERIOD, CAPLOK | CAPLOKALTGR | KANALOK, L'ю', L'Ю', WCH_NONE, L'ѭ', L'Ѭ'	, '.' ,  '>'},
	{VK_OEM_2     ,                        KANALOK,  '.',  ',', WCH_NONE,  ':',  ';'	, '/' ,  '?'},
	{VK_OEM_102   ,          CAPLOKALTGR | KANALOK, '\\',  '/', WCH_FS  , L'ꙉ', L'Ꙉ'	,'\\' ,  '|'},
	{0}
};

// Put this last so that VkKeyScan interprets number characters
// as coming from the main section of the kbd (aVkToWch0 and aVkToWch5)
// before considering the numpad (aVkToWch1).
static ALLOC_SECTION_LDATA VK_TO_WCHARS4 aVkToWch5[] = {
//	              │    │  Shift  │ Control │AltGr│
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

static ALLOC_SECTION_LDATA VK_F VkToFuncTable[] = {
	{
		VK_SCROLL,
		KBDNLS_INDEX_NORMAL,
		KBDNLS_TYPE_TOGGLE, 0b01100000,
		{
			{KBDNLS_SEND_BASE_VK , 0},
			{KBDNLS_SEND_PARAM_VK, VK_KANA},
			{KBDNLS_SEND_BASE_VK , 0},
			{KBDNLS_SEND_BASE_VK , 0},
			{KBDNLS_KANALOCK     , 0},
			{KBDNLS_SEND_PARAM_VK, VK_KANA},
			{KBDNLS_KANALOCK     , 0},
			{KBDNLS_SEND_BASE_VK , 0},
		},{
			{KBDNLS_SEND_BASE_VK , 0},
			{KBDNLS_SEND_PARAM_VK, VK_KANA},
			{KBDNLS_SEND_BASE_VK , 0},
			{KBDNLS_SEND_BASE_VK , 0},
			{KBDNLS_KANALOCK     , 0},
			{KBDNLS_SEND_PARAM_VK, VK_KANA},
			{KBDNLS_KANALOCK     , 0},
			{KBDNLS_SEND_BASE_VK , 0},
		}
	},
};

/***********************************************************************\
* KbdNlsTables
*
\***********************************************************************/
ALLOC_SECTION_LDATA KBDNLSTABLES KbdNlsTables = {
	NLSKBD_OEM_MICROSOFT, // OEM ID
	0,                    // Layout Information
	1,                    // Number of VK_F entry
	VkToFuncTable,        // Pointer to VK_F array
	0,                    // Number of MouseVk entry
	NULL                  // Pointer to MouseVk array
};

PKBDNLSTABLES KbdNlsLayerDescriptor(VOID) // @2
{
	return &KbdNlsTables;
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
