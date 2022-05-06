#pragma once

namespace Rose {
typedef enum class KeyCode : uint16_t
{
	// From glfw3.h
	Space = 32,
	Apostrophe = 39, /* ' */
	Comma = 44, /* , */
	Minus = 45, /* - */
	Period = 46, /* . */
	Slash = 47, /* / */

	D0 = 48, /* 0 */
	D1 = 49, /* 1 */
	D2 = 50, /* 2 */
	D3 = 51, /* 3 */
	D4 = 52, /* 4 */
	D5 = 53, /* 5 */
	D6 = 54, /* 6 */
	D7 = 55, /* 7 */
	D8 = 56, /* 8 */
	D9 = 57, /* 9 */

	Semicolon = 59, /* ; */
	Equal = 61, /* = */

	A = 65,
	B = 66,
	C = 67,
	D = 68,
	E = 69,
	F = 70,
	G = 71,
	H = 72,
	I = 73,
	J = 74,
	K = 75,
	L = 76,
	M = 77,
	N = 78,
	O = 79,
	P = 80,
	Q = 81,
	R = 82,
	S = 83,
	T = 84,
	U = 85,
	V = 86,
	W = 87,
	X = 88,
	Y = 89,
	Z = 90,

	LeftBracket = 91,  /* [ */
	Backslash = 92,  /* \ */
	RightBracket = 93,  /* ] */
	GraveAccent = 96,  /* ` */

	World1 = 161, /* non-US #1 */
	World2 = 162, /* non-US #2 */

	/* Function keys */
	Escape = 256,
	Enter = 257,
	Tab = 258,
	Backspace = 259,
	Insert = 260,
	Delete = 261,
	Right = 262,
	Left = 263,
	Down = 264,
	Up = 265,
	PageUp = 266,
	PageDown = 267,
	Home = 268,
	End = 269,
	CapsLock = 280,
	ScrollLock = 281,
	NumLock = 282,
	PrintScreen = 283,
	Pause = 284,
	F1 = 290,
	F2 = 291,
	F3 = 292,
	F4 = 293,
	F5 = 294,
	F6 = 295,
	F7 = 296,
	F8 = 297,
	F9 = 298,
	F10 = 299,
	F11 = 300,
	F12 = 301,
	F13 = 302,
	F14 = 303,
	F15 = 304,
	F16 = 305,
	F17 = 306,
	F18 = 307,
	F19 = 308,
	F20 = 309,
	F21 = 310,
	F22 = 311,
	F23 = 312,
	F24 = 313,
	F25 = 314,

	/* Keypad */
	KP0 = 320,
	KP1 = 321,
	KP2 = 322,
	KP3 = 323,
	KP4 = 324,
	KP5 = 325,
	KP6 = 326,
	KP7 = 327,
	KP8 = 328,
	KP9 = 329,
	KPDecimal = 330,
	KPDivide = 331,
	KPMultiply = 332,
	KPSubtract = 333,
	KPAdd = 334,
	KPEnter = 335,
	KPEqual = 336,

	LeftShift = 340,
	LeftControl = 341,
	LeftAlt = 342,
	LeftSuper = 343,
	RightShift = 344,
	RightControl = 345,
	RightAlt = 346,
	RightSuper = 347,
	Menu = 348
} Key;

static const KeyCode AllKeys[] = { 
Key::Space,Key::Apostrophe,Key::Comma,Key::Minus,Key::Period,Key::Slash,

Key::D0,Key::D1,Key::D2,Key::D3,Key::D4,Key::D5,Key::D6,Key::D7,Key::D8,Key::D9,Key::Semicolon,Key::Equal,
Key::A,Key::B,Key::C,Key::D,Key::E,Key::F,Key::G,Key::H,Key::I,Key::J,Key::K,Key::L,Key::M,Key::N,Key::O,Key::P,Key::Q,Key::R,Key::S,Key::T,Key::U,
Key::V,Key::W,Key::X,Key::Y,Key::Z,

Key::LeftBracket,Key::Backslash,Key::RightBracket,Key::GraveAccent,Key::World1,Key::World2,
Key::Escape,Key::Enter,Key::Tab,Key::Backspace,Key::Insert,Key::Delete,Key::Right,Key::Left,Key::Down,Key::Up,Key::PageUp,Key::PageDown,
Key::Home,Key::End,Key::CapsLock,Key::ScrollLock,Key::NumLock,Key::PrintScreen,Key::Pause,

Key::F1,Key::F2,Key::F3,Key::F4,Key::F5,Key::F6,Key::F7,Key::F8,Key::F9,Key::F10,Key::F11,Key::F12,Key::F13,Key::F14,Key::F15,
Key::F16,Key::F17,Key::F18,Key::F19,Key::F20,Key::F21,Key::F22,Key::F23,Key::F24,Key::F25,

Key::KP0,Key::KP1,Key::KP2,Key::KP3,Key::KP4,Key::KP5,Key::KP6,Key::KP7,Key::KP8,Key::KP9,

Key::KPDecimal,Key::KPDivide,Key::KPMultiply,Key::KPSubtract,Key::KPAdd,Key::KPEnter,Key::KPEqual,
Key::LeftShift,Key::LeftControl,Key::LeftAlt,Key::LeftSuper,Key::RightShift,Key::RightControl,Key::RightAlt,Key::RightSuper,Key::Menu
};

static std::string KeyToString(Key key) {
	switch (key) {
	case Key::Space: return "Space"; case Key::Apostrophe: return "apostrophe"; case Key::Comma: return "comma";
	case Key::Minus: return "minus"; case Key::Period: return "period"; case Key::Slash: return "slash";
	case Key::D0: return "0"; case Key::D1: return "1"; case Key::D2: return "2"; case Key::D3: return "3"; case Key::D4: return "4"; 
	case Key::D5: return "5"; case Key::D6: return "6"; case Key::D7: return "7"; case Key::D8: return "8"; case Key::D9: return "9";
	case Key::Semicolon: return "semicolon"; case Key::Equal: return "equal";

	case Key::A: return "a"; case Key::B: return "b"; case Key::C: return "c"; case Key::D: return "d"; case Key::E: return "e"; case Key::F: return "f"; 
	case Key::G: return "g"; case Key::H: return "h"; case Key::I: return "i"; case Key::J: return "j"; case Key::K: return "k"; case Key::L: return "l"; 
	case Key::M: return "m"; case Key::N: return "n"; case Key::O: return "o"; case Key::P: return "p"; case Key::Q: return "q"; case Key::R: return "r"; 
	case Key::S: return "s"; case Key::T: return "t"; case Key::U: return "u"; case Key::V: return "v"; case Key::W: return "w"; case Key::X: return "x"; 
	case Key::Y: return "y"; case Key::Z: return "z";

	case Key::LeftBracket: return "left_bracket"; case Key::Backslash: return "backslash"; case Key::RightBracket:  return "right_bracket";
	case Key::GraveAccent: return "grave_accent"; case Key::World1: return "world1"; case Key::World2: return "world2"; case Key::Escape: return "escape"; 
	case Key::Enter: return "enter"; case Key::Tab: return "tab"; case Key::Backspace: return "backspace"; case Key::Insert: return "insert"; 
	case Key::Delete: return "Delete";

	case Key::Left: return "left"; case Key::Right: return "right"; case Key::Up: return "up"; case Key::Down: return "down";

	case Key::PageUp: return "pageup"; case Key::PageDown: return "pagedown"; case Key::Home: return "home"; case Key::End: return "end";
	case Key::CapsLock: return "capslock"; case Key::ScrollLock: return "scroll_lock"; case Key::NumLock: "numlock"; case Key::PrintScreen: return "printscreen"; 
	case Key::Pause: return "pause";

	case Key::F1: return "f1"; case Key::F2: return "f2"; case Key::F3: return "f3"; case Key::F4: return "f4"; case Key::F5: return "f5";
	case Key::F6: return "f6"; case Key::F7: return "f7"; case Key::F8: return "f8"; case Key::F9: return "f9"; case Key::F10: return "f10"; 
	case Key::F11: return "f11"; case Key::F12: return "f12"; case Key::F13: return "f13"; case Key::F14: return "f14"; case Key::F15: return "f15";
	case Key::F16: return "f16"; case Key::F17: return "f17"; case Key::F18: return "f18"; case Key::F19: return "f19"; case Key::F20: return "f20";
	case Key::F21: return "f21"; case Key::F22: return "f22"; case Key::F23: return "f23"; case Key::F24: return "f24"; case Key::F25: return "f25";

	case Key::KP0: return "kp_0"; case Key::KP1: return "kp_1"; case Key::KP2: return "kp_2"; case Key::KP3: return "kp_3"; case Key::KP4: return "kp_4";
	case Key::KP5: return "kp_5"; case Key::KP6: return "kp_6"; case Key::KP7: return "kp_7"; case Key::KP8: return "kp_8"; case Key::KP9: return "kp_9";

	case Key::KPDecimal: return "kp_decimal"; case Key::KPDivide: return "kp_divide"; case Key::KPMultiply: return "kp_multiply";
	case Key::KPSubtract: return "kp_subtract"; case Key::KPAdd: return "kp_add"; case Key::KPEnter: return "kp_enter"; case Key::KPEqual: return "kp_equal";

	case Key::LeftShift: return "left_shift"; case Key::LeftControl: return "left_control"; case Key::LeftAlt: return "left_alt"; 
	case Key::LeftSuper: return "left_super"; case Key::RightShift: return "right_shift"; case Key::RightControl: return "right_control";
	case Key::RightAlt: return "right_alt"; case Key::RightSuper: return "right_super"; case Key::Menu: return "menu"; default: break; }

	RR_CORE_WARN("Key {0} is not mapped to a string!", key);
	return "unmapped";
}
inline std::ostream& operator<<(std::ostream& os, KeyCode keyCode)
{
	os << static_cast<int32_t>(keyCode);
	return os;
}
}
// From glfw3.h
#define RR_KEY_SPACE           ::Rose::Key::Space
#define RR_KEY_APOSTROPHE      ::Rose::Key::Apostrophe    /* ' */
#define RR_KEY_COMMA           ::Rose::Key::Comma         /* , */
#define RR_KEY_MINUS           ::Rose::Key::Minus         /* - */
#define RR_KEY_PERIOD          ::Rose::Key::Period        /* . */
#define RR_KEY_SLASH           ::Rose::Key::Slash         /* / */
#define RR_KEY_0               ::Rose::Key::D0
#define RR_KEY_1               ::Rose::Key::D1
#define RR_KEY_2               ::Rose::Key::D2
#define RR_KEY_3               ::Rose::Key::D3
#define RR_KEY_4               ::Rose::Key::D4
#define RR_KEY_5               ::Rose::Key::D5
#define RR_KEY_6               ::Rose::Key::D6
#define RR_KEY_7               ::Rose::Key::D7
#define RR_KEY_8               ::Rose::Key::D8
#define RR_KEY_9               ::Rose::Key::D9
#define RR_KEY_SEMICOLON       ::Rose::Key::Semicolon     /* ; */
#define RR_KEY_EQUAL           ::Rose::Key::Equal         /* = */
#define RR_KEY_A               ::Rose::Key::A
#define RR_KEY_B               ::Rose::Key::B
#define RR_KEY_C               ::Rose::Key::C
#define RR_KEY_D               ::Rose::Key::D
#define RR_KEY_E               ::Rose::Key::E
#define RR_KEY_F               ::Rose::Key::F
#define RR_KEY_G               ::Rose::Key::G
#define RR_KEY_H               ::Rose::Key::H
#define RR_KEY_I               ::Rose::Key::I
#define RR_KEY_J               ::Rose::Key::J
#define RR_KEY_K               ::Rose::Key::K
#define RR_KEY_L               ::Rose::Key::L
#define RR_KEY_M               ::Rose::Key::M
#define RR_KEY_N               ::Rose::Key::N
#define RR_KEY_O               ::Rose::Key::O
#define RR_KEY_P               ::Rose::Key::P
#define RR_KEY_Q               ::Rose::Key::Q
#define RR_KEY_R               ::Rose::Key::R
#define RR_KEY_S               ::Rose::Key::S
#define RR_KEY_T               ::Rose::Key::T
#define RR_KEY_U               ::Rose::Key::U
#define RR_KEY_V               ::Rose::Key::V
#define RR_KEY_W               ::Rose::Key::W
#define RR_KEY_X               ::Rose::Key::X
#define RR_KEY_Y               ::Rose::Key::Y
#define RR_KEY_Z               ::Rose::Key::Z
#define RR_KEY_LEFT_BRACKET    ::Rose::Key::LeftBracket   /* [ */
#define RR_KEY_BACKSLASH       ::Rose::Key::Backslash     /* \ */
#define RR_KEY_RIGHT_BRACKET   ::Rose::Key::RightBracket  /* ] */
#define RR_KEY_GRAVE_ACCENT    ::Rose::Key::GraveAccent   /* ` */
#define RR_KEY_WORLD_1         ::Rose::Key::World1        /* non-US #1 */
#define RR_KEY_WORLD_2         ::Rose::Key::World2        /* non-US #2 */
								 			 
/* Function keys */				 			 
#define RR_KEY_ESCAPE          ::Rose::Key::Escape
#define RR_KEY_ENTER           ::Rose::Key::Enter
#define RR_KEY_TAB             ::Rose::Key::Tab
#define RR_KEY_BACKSPACE       ::Rose::Key::Backspace
#define RR_KEY_INSERT          ::Rose::Key::Insert
#define RR_KEY_DELETE          ::Rose::Key::Delete
#define RR_KEY_RIGHT           ::Rose::Key::Right
#define RR_KEY_LEFT            ::Rose::Key::Left
#define RR_KEY_DOWN            ::Rose::Key::Down
#define RR_KEY_UP              ::Rose::Key::Up
#define RR_KEY_PAGE_UP         ::Rose::Key::PageUp
#define RR_KEY_PAGE_DOWN       ::Rose::Key::PageDown
#define RR_KEY_HOME            ::Rose::Key::Home
#define RR_KEY_END             ::Rose::Key::End
#define RR_KEY_CAPS_LOCK       ::Rose::Key::CapsLock
#define RR_KEY_SCROLL_LOCK     ::Rose::Key::ScrollLock
#define RR_KEY_NUM_LOCK        ::Rose::Key::NumLock
#define RR_KEY_PRINT_SCREEN    ::Rose::Key::PrintScreen
#define RR_KEY_PAUSE           ::Rose::Key::Pause
#define RR_KEY_F1              ::Rose::Key::F1
#define RR_KEY_F2              ::Rose::Key::F2
#define RR_KEY_F3              ::Rose::Key::F3
#define RR_KEY_F4              ::Rose::Key::F4
#define RR_KEY_F5              ::Rose::Key::F5
#define RR_KEY_F6              ::Rose::Key::F6
#define RR_KEY_F7              ::Rose::Key::F7
#define RR_KEY_F8              ::Rose::Key::F8
#define RR_KEY_F9              ::Rose::Key::F9
#define RR_KEY_F10             ::Rose::Key::F10
#define RR_KEY_F11             ::Rose::Key::F11
#define RR_KEY_F12             ::Rose::Key::F12
#define RR_KEY_F13             ::Rose::Key::F13
#define RR_KEY_F14             ::Rose::Key::F14
#define RR_KEY_F15             ::Rose::Key::F15
#define RR_KEY_F16             ::Rose::Key::F16
#define RR_KEY_F17             ::Rose::Key::F17
#define RR_KEY_F18             ::Rose::Key::F18
#define RR_KEY_F19             ::Rose::Key::F19
#define RR_KEY_F20             ::Rose::Key::F20
#define RR_KEY_F21             ::Rose::Key::F21
#define RR_KEY_F22             ::Rose::Key::F22
#define RR_KEY_F23             ::Rose::Key::F23
#define RR_KEY_F24             ::Rose::Key::F24
#define RR_KEY_F25             ::Rose::Key::F25
								 			 
/* Keypad */					 			 
#define RR_KEY_KP_0            ::Rose::Key::KP0
#define RR_KEY_KP_1            ::Rose::Key::KP1
#define RR_KEY_KP_2            ::Rose::Key::KP2
#define RR_KEY_KP_3            ::Rose::Key::KP3
#define RR_KEY_KP_4            ::Rose::Key::KP4
#define RR_KEY_KP_5            ::Rose::Key::KP5
#define RR_KEY_KP_6            ::Rose::Key::KP6
#define RR_KEY_KP_7            ::Rose::Key::KP7
#define RR_KEY_KP_8            ::Rose::Key::KP8
#define RR_KEY_KP_9            ::Rose::Key::KP9
#define RR_KEY_KP_DECIMAL      ::Rose::Key::KPDecimal
#define RR_KEY_KP_DIVIDE       ::Rose::Key::KPDivide
#define RR_KEY_KP_MULTIPLY     ::Rose::Key::KPMultiply
#define RR_KEY_KP_SUBTRACT     ::Rose::Key::KPSubtract
#define RR_KEY_KP_ADD          ::Rose::Key::KPAdd
#define RR_KEY_KP_ENTER        ::Rose::Key::KPEnter
#define RR_KEY_KP_EQUAL        ::Rose::Key::KPEqual
								 			 
#define RR_KEY_LEFT_SHIFT      ::Rose::Key::LeftShift
#define RR_KEY_LEFT_CONTROL    ::Rose::Key::LeftControl
#define RR_KEY_LEFT_ALT        ::Rose::Key::LeftAlt
#define RR_KEY_LEFT_SUPER      ::Rose::Key::LeftSuper
#define RR_KEY_RIGHT_SHIFT     ::Rose::Key::RightShift
#define RR_KEY_RIGHT_CONTROL   ::Rose::Key::RightControl
#define RR_KEY_RIGHT_ALT       ::Rose::Key::RightAlt
#define RR_KEY_RIGHT_SUPER     ::Rose::Key::RightSuper
#define RR_KEY_MENU            ::Rose::Key::Menu