#pragma once

#include "entt.hpp"

#include <glm/glm.hpp>

#include "RoseRoot/Core/UUID.h"
#include "RoseRoot/Core/Timestep.h"
#include "RoseRoot/Renderer/EditorCamera.h"

class b2World;

namespace Rose
{

	class Entity;

	class Scene {
	public:
		Scene();
		~Scene();

		static Ref<Scene> Copy(Ref<Scene> other);

		Entity CreateEntity(const std::string& name = std::string());
		Entity CreateEntityWithUUID(UUID uuid, const std::string& name = std::string());
		void DestroyEntity(Entity entity);

		void OnRuntimeStart(const std::string& assetPath);
		void OnRuntimeStop();

		void OnUpdateRuntime(Timestep ts, bool pause = false);
		void OnUpdateEditor(Timestep ts, EditorCamera& camera);
		void OnViewportResize(uint32_t width, uint32_t height);

		void DuplicateEntity(Entity entity);

		glm::vec2 GetGravity2D() { return m_SceneSettings.Gravity2D; }
		void SetGravity2D(glm::vec2 gravity) { m_SceneSettings.Gravity2D = gravity;}

		std::string GetName() { return m_SceneSettings.Name; }
		void SetName(std::string name) { m_SceneSettings.Name = name;}

		Entity GetPrimaryCameraEntity();

		template<typename... Components>
		auto GetAllEntitiesWith()
		{
			return m_Registry.view<Components...>();
		}
	private:
		template<typename T>
		void OnComponentAdded(Entity entity, T& component);
	private:
		struct SceneSettings
		{
			std::string Name;
			glm::vec2 Gravity2D = { 0.0f, -9.8f };

			SceneSettings() = default;
			SceneSettings(const SceneSettings&) = default;
		};

		SceneSettings m_SceneSettings;
		
		entt::registry m_Registry;
		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

		b2World* m_PhysicsWorld = nullptr;

		friend class Entity;
		friend class SceneSerializer;
		friend class SceneHierarchyPanel;
	};
}