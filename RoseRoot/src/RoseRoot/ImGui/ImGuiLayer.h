#pragma once

#include "RoseRoot/Core/Layer.h"

#include "RoseRoot/Events/KeyEvent.h"
#include "RoseRoot/Events/MouseEvent.h"
#include "RoseRoot/Events/ApplicationEvent.h"

namespace Rose {

	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnEvent(Event& e) override;

		void Begin();
		void End();

		void BlockEvents(bool block) { m_BlockEvents = block; }

		void SetDarkThemeColors();
	private:
		bool m_BlockEvents = true;
		float m_Time = 0;
	};
}