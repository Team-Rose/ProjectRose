#include "rrpch.h"
#include "Mesh.h"


namespace Rose {
	Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t> indices)
	{
		m_VertexArray = VertexArray::Create();
		Ref<VertexBuffer> vertexBuffer = VertexBuffer::Create(vertices.size() * sizeof(Vertex));
		vertexBuffer->SetData(vertices.data(), vertices.size() * sizeof(Vertex));
		vertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float3, "a_Normal" },
			{ ShaderDataType::Float2, "a_TexCoord" }
			});
		m_VertexArray->AddVertexBuffer(vertexBuffer);

		std::vector<uint32_t> flippedIndices;
		for (uint32_t i = 0; i < indices.size(); i++) {
			flippedIndices.push_back(indices.at((indices.size() - 1) - i));
		}

		Ref<IndexBuffer> indexBuffer;
		indexBuffer = IndexBuffer::Create(flippedIndices.data(), flippedIndices.size());
		m_VertexArray->SetIndexBuffer(indexBuffer);
	}
}

