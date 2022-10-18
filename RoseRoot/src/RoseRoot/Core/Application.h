#pragma once

#include "Core.h"

#include "Window.h"
#include "RoseRoot/Core/LayerStack.h"
#include "RoseRoot/Events/Event.h"
#include "RoseRoot/Events/ApplicationEvent.h"

#include "RoseRoot/Core/Timestep.h"

#include "RoseRoot/ImGui/ImGuiLayer.h"

namespace Rose {
	
	struct ApplicationCommandLineArgs
	{
		int Count = 0;
		char** Args = nullptr;

		const char* operator[](int index) const
		{
			RR_CORE_ASSERT(index < Count);
			return Args[index];
		}
	};

	class Application
	{
	public:
		Application(const std::string& name = "Rose App", ApplicationCommandLineArgs args = ApplicationCommandLineArgs());
		virtual ~Application();

		void Run();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		inline Window& GetWindow() { return *m_Window;  }

		void Close() { m_Running = false; }

		ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }

		static Application& Get() { return *s_Instance;  }

		ApplicationCommandLineArgs GetCommandLineArgs() const { return m_CommandLineArgs; }

		void SubmitToMainThread(const std::function<void()>& function);
	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

		void ExecuteMainThreadQueue();
	private:
		ApplicationCommandLineArgs m_CommandLineArgs;
		Scope<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		bool m_Minimized = false;
		LayerStack m_LayerStack;
		float m_LastFrameTime = 0.f;

		std::vector<std::function<void()>> m_MainThreadQueue;
		std::mutex m_MainThreadQueueMutex;
	private:
		static Application* s_Instance;
	};

	// To be defined in CLIENT
	Application* CreateApplication(ApplicationCommandLineArgs args);

}