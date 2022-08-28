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
	class ContactListener2D;

	struct SceneStats {
		//Timers are in MS
		float MonoScriptTime = 0.0f;
		float LuaSciptTime = 0.0f;
		float TotalScriptTime = 0.0f;

		float PhysicsTime = 0.0f;
		float RenderTime = 0.0f;
	};

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

		void OnSimulationStart(const std::string& assetPath);
		void OnSimulationStop();

		void OnUpdateRuntime(Timestep ts, bool pause = false);
		void OnUpdateSimulation(Timestep ts, EditorCamera& camera, bool pause = false);
		void OnUpdateEditor(Timestep ts, EditorCamera& camera);
		void OnViewportResize(uint32_t width, uint32_t height);

		Entity DuplicateEntity(Entity entity);

		Entity GetEntityByUUID(UUID uuid);

		glm::vec2 GetGravity2D() { return m_SceneSettings.Gravity2D; }
		void SetGravity2D(glm::vec2 gravity) { m_SceneSettings.Gravity2D = gravity;}

		std::string GetName() { return m_SceneSettings.Name; }
		void SetName(std::string name) { m_SceneSettings.Name = name;}

		SceneStats GetSceneStats() { return m_SceneStats; }

		Entity GetPrimaryCameraEntity();

		template<typename... Components>
		auto GetAllEntitiesWith()
		{
			return m_Registry.view<Components...>();
		}
	private:
		template<typename T>
		void OnComponentAdded(Entity entity, T& component);

		void OnPhysics2DStart();
		void OnPhysics2DStop();

		void RenderScene(EditorCamera& camera);
	private:
		struct SceneSettings
		{
			std::string Name;
			glm::vec2 Gravity2D = { 0.0f, -9.8f };

			SceneSettings() = default;
			SceneSettings(const SceneSettings&) = default;
		};

		SceneSettings m_SceneSettings;
		SceneStats m_SceneStats;
		entt::registry m_Registry;
		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

		b2World* m_PhysicsWorld = nullptr;
		ContactListener2D* m_Contactlistener;
		std::unordered_map<UUID, entt::entity> m_EntityMap;

		friend class Entity;
		friend class SceneSerializer;
		friend class SceneHierarchyPanel;
	};
}