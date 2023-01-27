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
		Rose::Ref<Rose::Texture2D> wood;
		Rose::Ref<Rose::Texture2D> woodspec;

		Rose::Ref<Rose::Texture2D> crate;
		Rose::Ref<Rose::Texture2D> cratespec;

		Rose::Mesh m_TestMesh;
		Rose::EditorCamera m_EditorCamera;
	};
}