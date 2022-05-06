#pragma once
#include <RoseRoot.h>
//---Entry Point---
#include <RoseRoot/Core/EntryPoint.h>
//-----------------

#include "MainLayer.h"


 {
	class RoseStem : public Application
	{
	public:
		RoseStem(ApplicationCommandLineArgs args)
			: Application("Sandbox", args)
		{
			PushLayer(new Sandbox::MainLayer());
		}

		~RoseStem()
		{

		}
	};

	Application* CreateApplication(ApplicationCommandLineArgs args)
	{
		return new RoseStem(args);
	}
}