#pragma once

#ifdef RR_PLATFORM_WINDOWS 

extern 
Rose::Application* Rose::CreateApplication(ApplicationCommandLineArgs args);

int main(int argc, char** argv)
{
	Rose::Log::Init();

	RR_PROFILE_BEGIN_SESSION("Startup", "RoseProfile-Startup.json");
	auto app = Rose::CreateApplication({ argc, argv });
	RR_PROFILE_END_SESSION();

	RR_PROFILE_BEGIN_SESSION("Runtime", "RoseProfile-Runtime.json");
	app->Run();
	RR_PROFILE_END_SESSION();

	RR_PROFILE_BEGIN_SESSION("Shutdown", "RoseProfile-Shutdown.json");
	delete app;
	RR_PROFILE_END_SESSION();
}

#endif