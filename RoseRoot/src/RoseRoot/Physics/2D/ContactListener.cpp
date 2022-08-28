#include "rrpch.h"
#include "ContactListener.h"

#include "RoseRoot/Scripting/Mono/MonoScriptEngine.h"

namespace Rose {
	namespace Utils {
		static void callOnCollision2DBegin(Entity entity, Entity other) {
			if (!entity.HasComponent<MonoScriptComponent>())
				return;
			
			MonoScriptEngine::OnCollision2DBeginInternal(entity, other.GetUUID());
		}

		static void callOnCollision2DEnd(Entity entity, Entity other) {
			if (!entity.HasComponent<MonoScriptComponent>())
				return;

			MonoScriptEngine::OnCollision2DEndInternal(entity, other.GetUUID());
		}
	}

	void ContactListener2D::BeginContact(b2Contact* contact)
	{
		if (MonoScriptEngine::GetSceneContext() != nullptr) {
			UUID IDA = (UUID)contact->GetFixtureA()->GetBody()->GetUserData().pointer;
			UUID IDB = (UUID)contact->GetFixtureB()->GetBody()->GetUserData().pointer;

			Entity a = MonoScriptEngine::GetSceneContext()->GetEntityByUUID(IDA);
			Entity b = MonoScriptEngine::GetSceneContext()->GetEntityByUUID(IDB);

			Utils::callOnCollision2DBegin(a, b);
			Utils::callOnCollision2DBegin(b, a);
		}
	}

	void ContactListener2D::EndContact(b2Contact* contact)
	{
		if (MonoScriptEngine::GetSceneContext() != nullptr) {
			UUID IDA = (UUID)contact->GetFixtureA()->GetBody()->GetUserData().pointer;
			UUID IDB = (UUID)contact->GetFixtureB()->GetBody()->GetUserData().pointer;

			Entity a = MonoScriptEngine::GetSceneContext()->GetEntityByUUID(IDA);
			Entity b = MonoScriptEngine::GetSceneContext()->GetEntityByUUID(IDB);

			Utils::callOnCollision2DEnd(a, b);
			Utils::callOnCollision2DEnd(b, a);
		}
	}
	void ContactListener2D::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
	{
		
		B2_NOT_USED(contact);
		B2_NOT_USED(oldManifold);
		
	}
	void ContactListener2D::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
	{
		{
			B2_NOT_USED(contact);
			B2_NOT_USED(impulse);
		}
	}
}

