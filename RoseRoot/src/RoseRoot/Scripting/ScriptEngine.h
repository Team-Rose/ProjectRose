#pragma once

namespace Rose {

	class ScriptEngine
	{
	public:

		static void Init();
		static void Shutdown();
	private:
		static void InitMono();
		static void ShutDownMono();

		static void InitLua();
	};
}