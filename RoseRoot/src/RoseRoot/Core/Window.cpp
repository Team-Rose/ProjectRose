#include "rrpch.h"
#include "RoseRoot/Core/Window.h"

#ifdef RR_PLATFORM_WINDOWS
#include "Platform/Windows/WindowsWindow.h"
#endif

namespace Rose
 {
	Scope<Window> Window::Create(const WindowProps& props)
	{
#ifdef RR_PLATFORM_WINDOWS
		return CreateScope<WindowsWindow>(props);
#else
		RR_CORE_ASSERT(false, "Unknown platform!");
		return nullptr;
#endif
	}
}