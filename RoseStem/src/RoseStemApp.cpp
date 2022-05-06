#pragma once
#include <RoseRoot.h>
//---Entry Point---
#include <RoseRoot/Core/EntryPoint.h>
//-----------------

#include "EditorLayer.h"

namespace Rose {
	class RoseStem : public Application
	{
	public:
		RoseStem(ApplicationCommandLineArgs args)
			: Application("Rose Stem", args)
		{
			PushLayer(new EditorLayer());
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
