#pragma once

#include "RoseRoot/Core/Window.h"
#include "RoseRoot/Renderer/GraphicsContexted.h"

#include <GLFW/glfw3.h>

namespace Rose
 {

	class WindowsWindow : public Window
	{
	public:
		WindowsWindow(const WindowProps& props);
		virtual ~WindowsWindow();

		void OnUpdate() override;
		virtual float GetTime() const override;

		inline unsigned int GetWidth() const override { return m_Data.Width; }
		inline unsigned int GetHeight() const override { return m_Data.Height; }

		virtual void SetWindowIcon(const std::string& path) const override;
		// Window attributes
		void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
		void SetVSync(bool enabled) override;
		bool IsVSync() const override;

		virtual void SetCapturesMouse(bool enabled) override;
		virtual bool GetCapturesMouse() const override;

		inline virtual void* GetNativeWindow() const { return m_Window; }
	private:
		virtual void Init(const WindowProps& props);
		virtual void Shutdown();
	private:
		GLFWwindow* m_Window;
		GraphicsContext* m_Context;
		struct WindowData
		{
			std::string Title;
			unsigned int Width, Height;
			bool VSync;
			bool CapturesMouse;

			EventCallbackFn EventCallback;
		};

		WindowData m_Data;
	};
}