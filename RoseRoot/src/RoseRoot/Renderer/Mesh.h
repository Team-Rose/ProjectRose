#pragma once
#include "VertexArray.h"

namespace Rose {
	struct Vertex
	{
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoord;
	};

	class Mesh {
	public:
		Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t> indices);

		const Ref<VertexArray> GetVertexArray() { return m_VertexArray; }
	private:
		Ref<VertexArray> m_VertexArray;
	};
}