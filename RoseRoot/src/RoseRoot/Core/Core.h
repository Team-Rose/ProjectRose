#pragma once

#include <memory>

#include "RoseRoot/Core/PlatformDetection.h"

#ifdef RR_DEBUG
#if defined(RR_PLATFORM_WINDOWS)
#define RR_DEBUGBREAK() __debugbreak()
#elif defined(RR_PLATFORM_LINUX)
#include <signal.h>
#define RR_DEBUGBREAK() raise(SIGTRAP)
#else
#error "Platform doesn't support debugbreak yet!"
#endif
#define RR_ENABLE_ASSERTS
#else
#define RR_DEBUGBREAK()
#endif

#define RR_EXPAND_MACRO(x) x
#define RR_STRINGIFY_MACRO(x) #x

#define BIT(x) (1 << x)

#define RR_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

namespace Rose {

	template<typename T>
	using Scope = std::unique_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
}