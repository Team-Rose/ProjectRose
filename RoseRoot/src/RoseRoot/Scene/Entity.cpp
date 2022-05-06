#include "rrpch.h"
#include "Entity.h"

namespace Rose {

	Entity::Entity(entt::entity handle, Scene* scene)
		: m_EntityHandle(handle), m_Scene(scene)
	{

	}
}