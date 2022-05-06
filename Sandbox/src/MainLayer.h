#pragma once
#include <RoseRoot.h>

namespace Sandbox {
	class MainLayer : public Rose::Layer {
	public:
		MainLayer();
		virtual ~MainLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnUpdate(Rose::Timestep ts) override;
		virtual void OnImGuiRender() override;
	private:
		//Rose::LuaScript m_Lua = Rose::LuaScript("assets/scripts/test.lua");
		//Rose::LuaScript m_Lua2 = Rose::LuaScript("assets/scripts/test2.lua");

		Rose::EditorCamera m_EditorCamera;
	};
}