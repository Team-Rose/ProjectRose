#include "rrpch.h"
#include "Mesh.h"

#define TINYGLTF_IMPLEMENTATION
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_STB_IMAGE_WRITE
#include <tiny_gltf.h>

//#include "assimp/"

namespace Rose {
	Mesh::Mesh(const std::filesystem::path& path)
	{
		bool valid = path.has_extension() && path.extension() != "glft" || path.extension() != "glb";
		if (!valid)
		{
			RR_CORE_WARN("Unsupported mesh format!");
			return;
		}
		tinygltf::Model model;
		tinygltf::TinyGLTF loader;
		std::string error;
		std::string warn;

		bool ret = loader.LoadBinaryFromFile(&model, &error, &warn, std::filesystem::absolute(path).string());
		if (!warn.empty())
			RR_CORE_WARN("GLTF loader had a warning: {}", warn);
		if (!error.empty())
			RR_CORE_ERROR("GLTF loader had a error: {}", error);

		if (!ret)
			RR_CORE_ERROR("Could not load GLTF or GLB file");
		
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;

		for each (tinygltf::Mesh mesh in model.meshes)
		{
			for each (tinygltf::Primitive primitive in mesh.primitives)
			{
				const tinygltf::Accessor& posAccessor = model.accessors[primitive.attributes["POSITION"]];
				const tinygltf::Accessor& normAccessor = model.accessors[primitive.attributes["NORMAL"]];
				const tinygltf::Accessor& coordAccessor = model.accessors[primitive.attributes["TEXCOORD_0"]];


				const tinygltf::BufferView& posBufferView = model.bufferViews[posAccessor.bufferView];
				const tinygltf::BufferView& normBufferView = model.bufferViews[normAccessor.bufferView];
				const tinygltf::BufferView& coordBufferView = model.bufferViews[coordAccessor.bufferView];


				const tinygltf::Buffer& posBuffer = model.buffers[posBufferView.buffer];
				const tinygltf::Buffer& normBuffer = model.buffers[normBufferView.buffer];
				const tinygltf::Buffer& coordBuffer = model.buffers[coordBufferView.buffer];

				const float* positions = reinterpret_cast<const float*>(&posBuffer.data[posBufferView.byteOffset + posAccessor.byteOffset]);
				const float* normals = reinterpret_cast<const float*>(&normBuffer.data[normBufferView.byteOffset + normAccessor.byteOffset]);
				const float* texCoords = reinterpret_cast<const float*>(&coordBuffer.data[coordBufferView.byteOffset + coordAccessor.byteOffset]);


				tinygltf::Accessor indexAccessor = model.accessors[primitive.indices];
				const tinygltf::BufferView& indexBufferView = model.bufferViews[indexAccessor.bufferView];
				const tinygltf::Buffer& indexBuffer = model.buffers[indexBufferView.buffer];

				const uint16_t* _indices = reinterpret_cast<const uint16_t*>(&indexBuffer.data[indexBufferView.byteOffset + indexAccessor.byteOffset]);

				indexAccessor.componentType;
				for (size_t i = 0; i < posAccessor.count; ++i) {
					vertices.push_back({
						glm::vec3(positions[i * 3 + 0], positions[i * 3 + 1], positions[i * 3 + 2]),
						glm::vec3(normals[i * 3 + 0], normals[i * 3 + 1], normals[i * 3 + 2]),
						glm::vec2(texCoords[i * 3 + 0], texCoords[i * 3 + 1])
						});

					indices.push_back((uint32_t)_indices[i]);
				}
			}
		}
		
		RR_CORE_TRACE("Indices: {}, Vertices: {}", indices.size(), vertices.size());
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
			flippedIndices.push_back(indices.at((indices.size()-1) - i));
		}
		Ref<IndexBuffer> indexBuffer;
		indexBuffer = IndexBuffer::Create(flippedIndices.data(), flippedIndices.size());
		m_VertexArray->SetIndexBuffer(indexBuffer);
	}
}

