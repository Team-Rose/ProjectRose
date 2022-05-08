#pragma once
#include <RoseRoot.h>
//---Entry Point---
#include <RoseRoot/Core/EntryPoint.h>
//-----------------

#include "MainLayer.h"

namespace Rose
 {
	class SandboxApp : public Application
	{
	public:
		SandboxApp(ApplicationCommandLineArgs args)
			: Application("Sandbox", args)
		{
			PushLayer(new Sandbox::MainLayer());
		}

		~SandboxApp()
		{

		}
	};

	Application* CreateApplication(ApplicationCommandLineArgs args)
	{
		return new SandboxApp(args);
	}
}