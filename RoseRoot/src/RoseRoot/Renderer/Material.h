#pragma once
#include "Shader.h"

namespace Rose {
	class Material {
	public:
		Material(Ref<Shader> shader);
	private:
		Ref<Shader> m_Shader;
	};
}