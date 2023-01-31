#include "rrpch.h"
#include "SceneSerializer.h"

#include "Entity.h"
#include "Components.h"
#include "RoseRoot/Assets/AssetManager.h"
#include "RoseRoot/Scripting/Mono/MonoScriptEngine.h"

#include <fstream>
#include <yaml-cpp/yaml.h>

namespace YAML {

	template<>
	struct convert<Rose::UUID>
	{
		static Node encode(const Rose::UUID& uuid)
		{
			Node node;
			node.push_back((uint64_t)uuid);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, Rose::UUID& uuid)
		{
			uuid = (Rose::UUID)node[0].as<uint64_t>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec2>
	{
		static Node encode(const glm::vec2& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec2& rhs)
		{
			if (!node.IsSequence() || node.size() != 2)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec3>
	{
		static Node encode(const glm::vec3& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec3& rhs)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec4>
	{
		static Node encode(const glm::vec4& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec4& rhs)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};

}

namespace Rose
{

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec2& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
		return out;
	}

	static std::string RigidBody2DBodyTypeToString(Rigidbody2DComponent::BodyType bodyType)
	{
		switch (bodyType)
		{
		case Rigidbody2DComponent::BodyType::Static:    return "Static";
		case Rigidbody2DComponent::BodyType::Dynamic:   return "Dynamic";
		case Rigidbody2DComponent::BodyType::Kinematic: return "Kinematic";
		}

		RR_CORE_ASSERT(false, "Unknown body type");
		return {};
	}

	static Rigidbody2DComponent::BodyType RigidBody2DBodyTypeFromString(const std::string& bodyTypeString)
	{
		if (bodyTypeString == "Static")    return Rigidbody2DComponent::BodyType::Static;
		if (bodyTypeString == "Dynamic")   return Rigidbody2DComponent::BodyType::Dynamic;
		if (bodyTypeString == "Kinematic") return Rigidbody2DComponent::BodyType::Kinematic;

		RR_CORE_ASSERT(false, "Unknown body type");
		return Rigidbody2DComponent::BodyType::Static;
	}

	SceneSerializer::SceneSerializer(const Ref<Scene>& scene)
		: m_Scene(scene)
	{
	}

	static void SerializeEntity(YAML::Emitter& out, Entity entity)
	{
		RR_CORE_ASSERT(entity.HasComponent<IDComponent>());

		out << YAML::BeginMap; // Entity
		out << YAML::Key << "Entity" << YAML::Value << entity.GetUUID();

		if (entity.HasComponent<TagComponent>())
		{
			out << YAML::Key << "TagComponent";
			out << YAML::BeginMap; // TagComponent

			auto& tag = entity.GetComponent<TagComponent>().Tag;
			out << YAML::Key << "Tag" << YAML::Value << tag;

			out << YAML::EndMap; // TagComponent
		}
		if (entity.HasComponent<RelationshipComponent>())
		{
			auto& parent = entity.GetComponent<RelationshipComponent>().ParentHandle;

			if (parent != 0) {
				out << YAML::Key << "RelationshipComponent";
				out << YAML::BeginMap; // RelationshipComponent

				out << YAML::Key << "Parent" << YAML::Value << parent;

				out << YAML::EndMap; // RelationshipComponent
			}
		}

		if (entity.HasComponent<TransformComponent>())
		{
			out << YAML::Key << "TransformComponent";
			out << YAML::BeginMap; // TransformComponent

			auto& tc = entity.GetComponent<TransformComponent>();
			out << YAML::Key << "Translation" << YAML::Value << tc.Translation;
			out << YAML::Key << "Rotation" << YAML::Value << tc.Rotation;
			out << YAML::Key << "Scale" << YAML::Value << tc.Scale;

			out << YAML::EndMap; // TransformComponent
		}

		if (entity.HasComponent<CameraComponent>())
		{
			out << YAML::Key << "CameraComponent";
			out << YAML::BeginMap; // CameraComponent

			auto& cameraComponent = entity.GetComponent<CameraComponent>();
			auto& camera = cameraComponent.Camera;

			out << YAML::Key << "Camera" << YAML::Value;
			out << YAML::BeginMap; // Camera
			out << YAML::Key << "ProjectionType" << YAML::Value << (int)camera.GetProjectionType();
			out << YAML::Key << "PerspectiveFOV" << YAML::Value << camera.GetPerspectiveVerticalFOV();
			out << YAML::Key << "PerspectiveNear" << YAML::Value << camera.GetPerspectiveNearClip();
			out << YAML::Key << "PerspectiveFar" << YAML::Value << camera.GetPerspectiveFarClip();
			out << YAML::Key << "OrthographicSize" << YAML::Value << camera.GetOrthographicSize();
			out << YAML::Key << "OrthographicNear" << YAML::Value << camera.GetOrthographicNearClip();
			out << YAML::Key << "OrthographicFar" << YAML::Value << camera.GetOrthographicFarClip();
			out << YAML::EndMap; // Camera

			out << YAML::Key << "Primary" << YAML::Value << cameraComponent.Primary;
			out << YAML::Key << "FixedAspectRatio" << YAML::Value << cameraComponent.FixedAspectRatio;

			out << YAML::EndMap; // CameraComponent
		}

		if (entity.HasComponent<SpriteRendererComponent>())
		{
			out << YAML::Key << "SpriteRendererComponent";
			out << YAML::BeginMap; // SpriteRendererComponent

			auto& spriteRendererComponent = entity.GetComponent<SpriteRendererComponent>();
			out << YAML::Key << "Color" << YAML::Value << spriteRendererComponent.Color;
			out << YAML::Key << "TilingFactor" << YAML::Value << spriteRendererComponent.TilingFactor;
			if (spriteRendererComponent.Path != "no_texture") {
				out << YAML::Key << "Path" << YAML::Value << spriteRendererComponent.Path;
			}

			out << YAML::EndMap; // SpriteRendererComponent
		}

		if (entity.HasComponent<MeshRendererComponent>())
		{
			out << YAML::Key << "MeshRendererComponent";
			out << YAML::BeginMap; // MeshRendererComponent

			auto& meshRendererComponent = entity.GetComponent<MeshRendererComponent>();
			out << YAML::Key << "Color" << YAML::Value << meshRendererComponent.Color;
			if (!meshRendererComponent.Path.empty()) {
				out << YAML::Key << "Path" << YAML::Value << meshRendererComponent.Path.string();
				out << YAML::Key << "MeshIndex" << YAML::Value << meshRendererComponent.MeshIndex;
			}
			if (meshRendererComponent.Path != "no_texture") {
				out << YAML::Key << "BaseTexturePath" << YAML::Value << meshRendererComponent.BaseTexturePath;
			}

			out << YAML::EndMap; // MeshRendererComponent
		}

		if (entity.HasComponent<CircleRendererComponent>())
		{
			out << YAML::Key << "CircleRendererComponent";
			out << YAML::BeginMap; // CircleRendererComponent

			auto& circleRendererComponent = entity.GetComponent<CircleRendererComponent>();
			out << YAML::Key << "Color" << YAML::Value << circleRendererComponent.Color;
			out << YAML::Key << "Thickness" << YAML::Value << circleRendererComponent.Thickness;
			out << YAML::Key << "Fade" << YAML::Value << circleRendererComponent.Fade;

			out << YAML::EndMap; // CircleRendererComponent
		}

		if (entity.HasComponent<Rigidbody2DComponent>())
		{
			out << YAML::Key << "Rigidbody2DComponent";
			out << YAML::BeginMap; // Rigidbody2DComponent

			auto& rb2dComponent = entity.GetComponent<Rigidbody2DComponent>();
			out << YAML::Key << "BodyType" << YAML::Value << RigidBody2DBodyTypeToString(rb2dComponent.Type);
			out << YAML::Key << "FixedRotation" << YAML::Value << rb2dComponent.FixedRotation;

			out << YAML::EndMap; // Rigidbody2DComponent
		}

		if (entity.HasComponent<BoxCollider2DComponent>())
		{
			out << YAML::Key << "BoxCollider2DComponent";
			out << YAML::BeginMap; // BoxCollider2DComponent

			auto& bc2dComponent = entity.GetComponent<BoxCollider2DComponent>();
			out << YAML::Key << "Offset" << YAML::Value << bc2dComponent.Offset;
			out << YAML::Key << "Size" << YAML::Value << bc2dComponent.Size;
			out << YAML::Key << "Density" << YAML::Value << bc2dComponent.Density;
			out << YAML::Key << "Friction" << YAML::Value << bc2dComponent.Friction;
			out << YAML::Key << "Restitution" << YAML::Value << bc2dComponent.Restitution;
			out << YAML::Key << "RestitutionThreshold" << YAML::Value << bc2dComponent.RestitutionThreshold;

			out << YAML::EndMap; // BoxCollider2DComponent
		}

		if (entity.HasComponent<CircleCollider2DComponent>())
		{
			out << YAML::Key << "CircleCollider2DComponent";
			out << YAML::BeginMap; // CircleCollider2DComponent

			auto& cc2dComponent = entity.GetComponent<CircleCollider2DComponent>();
			out << YAML::Key << "Offset" << YAML::Value << cc2dComponent.Offset;
			out << YAML::Key << "Radius" << YAML::Value << cc2dComponent.Radius;
			out << YAML::Key << "Density" << YAML::Value << cc2dComponent.Density;
			out << YAML::Key << "Friction" << YAML::Value << cc2dComponent.Friction;
			out << YAML::Key << "Restitution" << YAML::Value << cc2dComponent.Restitution;
			out << YAML::Key << "RestitutionThreshold" << YAML::Value << cc2dComponent.RestitutionThreshold;

			out << YAML::EndMap; // CircleCollider2DComponent
		}
		if (entity.HasComponent<LuaScriptComponent>())
		{
			out << YAML::Key << "LuaScriptComponent";
			out << YAML::BeginMap; // LuaScriptComponent

			auto& luaComponent = entity.GetComponent<LuaScriptComponent>();
			out << YAML::Key << "Path" << YAML::Value << luaComponent.Path;

			out << YAML::EndMap; // LuaScriptComponent
		}
		if (entity.HasComponent<MonoScriptComponent>())
		{
			out << YAML::Key << "MonoScriptComponent";
			out << YAML::BeginMap; // MonoScriptComponent

			auto& monoComponent = entity.GetComponent<MonoScriptComponent>();
			out << YAML::Key << "ClassName" << YAML::Value << monoComponent.ClassName;

			// Fields
			Ref<MonoScriptClass> entityClass = MonoScriptEngine::GetEntityClass(monoComponent.ClassName);
			const auto& fields = entityClass->GetFields();
			auto& entityFields = MonoScriptEngine::GetScriptFieldMap(entity.GetUUID());

			if (entityFields.size() != 0) {
				out << YAML::Key << "ScriptFields" << YAML::Value;
				out << YAML::BeginSeq; // MonoScriptFields

				for (const auto& [name, field] : fields) {
					// Field has been set in editor
					if (entityFields.find(name) != entityFields.end()) {

						out << YAML::BeginMap;
						out << YAML::Key << "Name" << YAML::Value << name;
						out << YAML::Key << "Type" << YAML::Value << Utils::MonoScriptFieldTypeToString(field.Type);

						MonoScriptFieldInstance& scriptFieldInstance = entityFields.at(name);

#define FIELD_TYPE(FieldType, Type)\
					case FieldType:		\
						out << YAML::Key << "Data" << YAML::Value << scriptFieldInstance.GetValue<Type>();		\
						break;

						switch (field.Type)
						{
							FIELD_TYPE(MonoScriptFieldType::Float,	 float    );
							FIELD_TYPE(MonoScriptFieldType::Double,  double	  );
																	 
							FIELD_TYPE(MonoScriptFieldType::Bool,	 bool	  );
							FIELD_TYPE(MonoScriptFieldType::Char,	 char	  );
																	 
							FIELD_TYPE(MonoScriptFieldType::Byte,	 int8_t   );
							FIELD_TYPE(MonoScriptFieldType::Short,   int16_t  );
							FIELD_TYPE(MonoScriptFieldType::Int,	 int	  );
							FIELD_TYPE(MonoScriptFieldType::Long,    int64_t  );
																	 
							FIELD_TYPE(MonoScriptFieldType::UByte,   uint8_t  );
							FIELD_TYPE(MonoScriptFieldType::UInt16,  uint16_t );
							FIELD_TYPE(MonoScriptFieldType::UInt32,  uint32_t );
							FIELD_TYPE(MonoScriptFieldType::UInt64,  uint64_t );
																	 
							FIELD_TYPE(MonoScriptFieldType::Vector2, glm::vec2);
							FIELD_TYPE(MonoScriptFieldType::Vector3, glm::vec3);
							FIELD_TYPE(MonoScriptFieldType::Vector4, glm::vec4);

							FIELD_TYPE(MonoScriptFieldType::Entity,	 UUID	  );
							
						}
#undef FIELD_TYPE
						out << YAML::EndMap;
					}
				}

				out << YAML::EndSeq; // MonoScriptFields
			}

			out << YAML::EndMap; // MonoScriptComponent
		}
		out << YAML::EndMap; // Entity
	}

	void SceneSerializer::Serialize(const std::string& filepath)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << m_Scene->m_SceneSettings.Name.c_str();

		out << YAML::Key << "Scene Settings";
		out << YAML::BeginMap;

		out << YAML::Key << "Gravity2D" << YAML::Value << m_Scene->m_SceneSettings.Gravity2D;

		out << YAML::EndMap;

		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
		m_Scene->m_Registry.each([&](auto entityID)
			{
				Entity entity = { entityID, m_Scene.get() };
				if (!entity)
					return;

				SerializeEntity(out, entity);
			});
		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::ofstream fout(filepath);
		fout << out.c_str();
	}

	void SceneSerializer::SerializeRuntime(const std::string& filepath)
	{
		// Not implemented
		RR_CORE_ASSERT(false);
	}

	bool SceneSerializer::Deserialize(const std::string& filepath, const std::string& assetPath)
	{
		YAML::Node data;
		try
		{
			data = YAML::LoadFile(filepath);
		}
		catch (YAML::ParserException e)
		{
			return false;
		}

		if (!data["Scene"])
			return false;

		std::string sceneName = data["Scene"].as<std::string>();

#ifdef RR_DEBUG
		RR_CORE_TRACE("Deserializing scene '{0}'", sceneName);
#endif

		auto sceneSettings = data["Scene Settings"];
		if (sceneSettings)
		{
			m_Scene->m_SceneSettings.Name = sceneName;

			auto Gravity2D = sceneSettings["Gravity2D"];
			if (Gravity2D)
			{
				glm::vec2 gravity = Gravity2D.as<glm::vec2>();
#ifdef RR_DEBUG
				RR_CORE_TRACE("Deserialized Gravity2D {0}, {1}", gravity.x, gravity.y);
#endif
				m_Scene->m_SceneSettings.Gravity2D = gravity;
			}
		}

		auto entities = data["Entities"];
		if (entities)
		{
			std::unordered_map<UUID, std::vector<UUID>> parentsToChildren;

			for (auto entity : entities)
			{
				uint64_t uuid = entity["Entity"].as<uint64_t>();

				std::string name;
				auto tagComponent = entity["TagComponent"];
				if (tagComponent)
					name = tagComponent["Tag"].as<std::string>();
#ifdef RR_DEBUG
				RR_CORE_TRACE("Deserialized entity with ID = {0}, name = {1}", uuid, name);
#endif
				Entity deserializedEntity = m_Scene->CreateEntityWithUUID(uuid, name);

				auto transformComponent = entity["TransformComponent"];
				if (transformComponent)
				{
					// Entities always have transforms
					auto& tc = deserializedEntity.GetComponent<TransformComponent>();
					tc.Translation = transformComponent["Translation"].as<glm::vec3>();
					tc.Rotation = transformComponent["Rotation"].as<glm::vec3>();
					tc.Scale = transformComponent["Scale"].as<glm::vec3>();
				}

				auto relationshipComponent = entity["RelationshipComponent"];
				if (relationshipComponent)
				{
					UUID parentID = (UUID)relationshipComponent["Parent"].as<uint64_t>();
					if (parentID != 0) {
						if (parentsToChildren.find(parentID) == parentsToChildren.end()) {
							parentsToChildren[parentID] = std::vector<UUID>();
						}
						parentsToChildren[parentID].push_back(deserializedEntity.GetUUID());
					}
				}

#pragma region OptionalComponents
				auto cameraComponent = entity["CameraComponent"];
				if (cameraComponent)
				{
					auto& cc = deserializedEntity.AddComponent<CameraComponent>();

					auto& cameraProps = cameraComponent["Camera"];
					cc.Camera.SetProjectionType((SceneCamera::ProjectionType)cameraProps["ProjectionType"].as<int>());

					cc.Camera.SetPerspectiveVerticalFOV(cameraProps["PerspectiveFOV"].as<float>());
					cc.Camera.SetPerspectiveNearClip(cameraProps["PerspectiveNear"].as<float>());
					cc.Camera.SetPerspectiveFarClip(cameraProps["PerspectiveFar"].as<float>());

					cc.Camera.SetOrthographicSize(cameraProps["OrthographicSize"].as<float>());
					cc.Camera.SetOrthographicNearClip(cameraProps["OrthographicNear"].as<float>());
					cc.Camera.SetOrthographicFarClip(cameraProps["OrthographicFar"].as<float>());

					cc.Primary = cameraComponent["Primary"].as<bool>();
					cc.FixedAspectRatio = cameraComponent["FixedAspectRatio"].as<bool>();
				}

				auto spriteRendererComponent = entity["SpriteRendererComponent"];
				if (spriteRendererComponent)
				{
					auto& src = deserializedEntity.AddComponent<SpriteRendererComponent>();
					src.Color = spriteRendererComponent["Color"].as<glm::vec4>();
					if (spriteRendererComponent["TilingFactor"]) {
						src.TilingFactor = spriteRendererComponent["TilingFactor"].as<float>();
					}
					if (spriteRendererComponent["Path"] && spriteRendererComponent["Path"].as<std::string>() != "no_texture") {
						src.Path = spriteRendererComponent["Path"].as<std::string>();
						if (Ref<Texture2D> texture = AssetManager::GetOrLoadTexture(src.Path))
							src.Texture = texture;
					}
				}

				auto meshRendererComponent = entity["MeshRendererComponent"];
				if (meshRendererComponent)
				{
					auto& src = deserializedEntity.AddComponent<MeshRendererComponent>();
					src.Color = meshRendererComponent["Color"].as<glm::vec4>();

					if (meshRendererComponent["Path"] && !meshRendererComponent["Path"].as<std::string>().empty()) {
						src.Path = meshRendererComponent["Path"].as<std::string>();

						if (Ref<Model> model = AssetManager::GetOrLoadModel(src.Path)) {
							src.Model = model;
							src.MeshIndex = meshRendererComponent["MeshIndex"] ? meshRendererComponent["MeshIndex"].as<int>() : 0;
						}
					}

					if (meshRendererComponent["BaseTexturePath"] && meshRendererComponent["BaseTexturePath"].as<std::string>() != "no_texture") {
						src.BaseTexturePath = meshRendererComponent["BaseTexturePath"].as<std::string>();
						if (Ref<Texture2D> texture = AssetManager::GetOrLoadTexture(src.BaseTexturePath))
							src.BaseTexture = texture;
					}
				}

				auto circleRendererComponent = entity["CircleRendererComponent"];
				if (circleRendererComponent)
				{
					auto& src = deserializedEntity.AddComponent<CircleRendererComponent>();
					src.Color = circleRendererComponent["Color"].as<glm::vec4>();
					src.Thickness = circleRendererComponent["Thickness"].as<float>();
					src.Fade = circleRendererComponent["Fade"].as<float>();
				}

				auto rigidbody2DComponent = entity["Rigidbody2DComponent"];
				if (rigidbody2DComponent)
				{
					auto& rb2d = deserializedEntity.AddComponent<Rigidbody2DComponent>();
					rb2d.Type = RigidBody2DBodyTypeFromString(rigidbody2DComponent["BodyType"].as<std::string>());
					rb2d.FixedRotation = rigidbody2DComponent["FixedRotation"].as<bool>();
				}

				auto boxCollider2DComponent = entity["BoxCollider2DComponent"];
				if (boxCollider2DComponent)
				{
					auto& bc2d = deserializedEntity.AddComponent<BoxCollider2DComponent>();
					bc2d.Offset = boxCollider2DComponent["Offset"].as<glm::vec2>();
					bc2d.Size = boxCollider2DComponent["Size"].as<glm::vec2>();
					bc2d.Density = boxCollider2DComponent["Density"].as<float>();
					bc2d.Friction = boxCollider2DComponent["Friction"].as<float>();
					bc2d.Restitution = boxCollider2DComponent["Restitution"].as<float>();
					bc2d.RestitutionThreshold = boxCollider2DComponent["RestitutionThreshold"].as<float>();
				}

				auto circleCollider2DComponent = entity["CircleCollider2DComponent"];
				if (circleCollider2DComponent)
				{
					auto& cc2d = deserializedEntity.AddComponent<CircleCollider2DComponent>();
					cc2d.Offset = circleCollider2DComponent["Offset"].as<glm::vec2>();
					cc2d.Radius = circleCollider2DComponent["Radius"].as<float>();
					cc2d.Density = circleCollider2DComponent["Density"].as<float>();
					cc2d.Friction = circleCollider2DComponent["Friction"].as<float>();
					cc2d.Restitution = circleCollider2DComponent["Restitution"].as<float>();
					cc2d.RestitutionThreshold = circleCollider2DComponent["RestitutionThreshold"].as<float>();
				}
				auto luaScriptComponent = entity["LuaScriptComponent"];
				if (luaScriptComponent)
				{
					auto& lsc = deserializedEntity.AddComponent<LuaScriptComponent>();
					lsc.Path = luaScriptComponent["Path"].as<std::string>();
				}

				auto monoScriptComponent = entity["MonoScriptComponent"];
				if (monoScriptComponent)
				{
					auto& msc = deserializedEntity.AddComponent<MonoScriptComponent>();
					msc.ClassName = monoScriptComponent["ClassName"].as<std::string>();

					auto scriptFields = monoScriptComponent["ScriptFields"];
					if (scriptFields)
					{
						Ref<MonoScriptClass> entityClass = MonoScriptEngine::GetEntityClass(msc.ClassName);
						if (entityClass) {
							const auto& fields = entityClass->GetFields();
							auto& entityFields = MonoScriptEngine::GetScriptFieldMap(deserializedEntity.GetUUID());

#define READ_FIELD_TYPE(FieldType, Type)	\
							case FieldType:		\
							{ Type data = scriptField["Data"].as<Type>(); \
							fieldInstance.SetValue(data); }	\
							break; 

							for (auto scriptField : scriptFields)
							{
								std::string name = scriptField["Name"].as<std::string>();
								std::string typeString = scriptField["Type"].as<std::string>();
								MonoScriptFieldType type = Utils::StringToMonoScriptFieldType(typeString);

								// TODO(Sam): turn this assert into RoseStem log warning
								RR_CORE_ASSERT(fields.find(name) != fields.end());

								if (fields.find(name) == fields.end())
									continue;

								MonoScriptFieldInstance& fieldInstance = entityFields[name];
								fieldInstance.Field = fields.at(name);

								switch (fieldInstance.Field.Type)
								{
									READ_FIELD_TYPE(MonoScriptFieldType::Float, float);
									READ_FIELD_TYPE(MonoScriptFieldType::Double, double);

									READ_FIELD_TYPE(MonoScriptFieldType::Bool, bool);
									READ_FIELD_TYPE(MonoScriptFieldType::Char, char);

									READ_FIELD_TYPE(MonoScriptFieldType::Byte, int8_t);
									READ_FIELD_TYPE(MonoScriptFieldType::Short, int16_t);
									READ_FIELD_TYPE(MonoScriptFieldType::Int, int);
									READ_FIELD_TYPE(MonoScriptFieldType::Long, int64_t);

									READ_FIELD_TYPE(MonoScriptFieldType::UByte, uint8_t);
									READ_FIELD_TYPE(MonoScriptFieldType::UInt16, uint16_t);
									READ_FIELD_TYPE(MonoScriptFieldType::UInt32, uint32_t);
									READ_FIELD_TYPE(MonoScriptFieldType::UInt64, uint64_t);

									READ_FIELD_TYPE(MonoScriptFieldType::Vector2, glm::vec2);
									READ_FIELD_TYPE(MonoScriptFieldType::Vector3, glm::vec3);
									READ_FIELD_TYPE(MonoScriptFieldType::Vector4, glm::vec4);

									READ_FIELD_TYPE(MonoScriptFieldType::Entity, UUID);
								}
							}
#undef WRITE_FIELD_TYPE
						} else {
							RR_WARN("No entity class found in provided binary {}", msc.ClassName);
						}
						
					}
				}
#pragma endregion

			}

			for (auto& [parentID, children] : parentsToChildren) {
				Entity parent = m_Scene->GetEntityByUUID(parentID);
				for (auto& childID : children) {
					m_Scene->ParentEntity(m_Scene->GetEntityByUUID(childID), parent);
				}
			}
		}

		return true;
	}

	bool SceneSerializer::DeserializeRuntime(const std::string& filepath)
	{
		// Not implemented
		RR_CORE_ASSERT(false);
		return false;
	}

}