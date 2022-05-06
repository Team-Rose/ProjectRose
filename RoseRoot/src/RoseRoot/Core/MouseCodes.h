#pragma once

namespace Rose
{
	typedef enum class MouseCode : uint16_t
	{
		// From glfw3.h
		Button0 = 0,
		Button1 = 1,
		Button2 = 2,
		Button3 = 3,
		Button4 = 4,
		Button5 = 5,
		Button6 = 6,
		Button7 = 7,

		ButtonLast = Button7,
		ButtonLeft = Button0,
		ButtonRight = Button1,
		ButtonMiddle = Button2
	} Mouse;

	static const MouseCode AllMouseButtons[] = {
		Mouse::Button0, Mouse::Button1, Mouse::Button2, Mouse::Button3, Mouse::Button4, Mouse::Button5, Mouse::Button6, Mouse::Button7,
	};

	static std::string MouseToString(Mouse button) { 
		switch (button)
		{
		case Mouse::Button0: return "left";
		case Mouse::Button1: return "right";
		case Mouse::Button2: return "button2";
		case Mouse::Button3: return "button3";
		case Mouse::Button4: return "button4";
		case Mouse::Button5: return "button5";
		case Mouse::Button6: return "button6";
		case Mouse::Button7: return "button7";

		default:
			break;
		}
		RR_CORE_WARN("Mouse Button {0} is not mapped to a string!", button);
		return "unmapped";
	}

	inline std::ostream& operator<<(std::ostream& os, MouseCode mouseCode)
	{
		os << static_cast<int32_t>(mouseCode);
		return os;
	}
}
//From glfw3.h
#define RR_MOUSE_BUTTON_0      ::Rose::Mouse::Button0
#define RR_MOUSE_BUTTON_1      ::Rose::Mouse::Button1
#define RR_MOUSE_BUTTON_2      ::Rose::Mouse::Button2
#define RR_MOUSE_BUTTON_3      ::Rose::Mouse::Button3
#define RR_MOUSE_BUTTON_4      ::Rose::Mouse::Button4
#define RR_MOUSE_BUTTON_5      ::Rose::Mouse::Button5
#define RR_MOUSE_BUTTON_6      ::Rose::Mouse::Button6
#define RR_MOUSE_BUTTON_7      ::Rose::Mouse::Button7
#define RR_MOUSE_BUTTON_LAST   ::Rose::Mouse::ButtonLast
#define RR_MOUSE_BUTTON_LEFT   ::Rose::Mouse::ButtonLeft
#define RR_MOUSE_BUTTON_RIGHT  ::Rose::Mouse::ButtonRight
#define RR_MOUSE_BUTTON_MIDDLE ::Rose::Mouse::ButtonMiddle