#include "rrpch.h"
#include "ContactListener.h"

#include "RoseRoot/Scripting/Mono/MonoScriptEngine.h"

namespace Rose {
	namespace Utils {

	}

	void ContactListener2D::BeginContact(b2Contact* contact)
	{
		if (MonoScriptEngine::GetSceneContext() != nullptr) {
			UUID IDA = (UUID)contact->GetFixtureA()->GetBody()->GetUserData().pointer;
			UUID IDB = (UUID)contact->GetFixtureB()->GetBody()->GetUserData().pointer;

			Entity a = MonoScriptEngine::GetSceneContext()->GetEntityByUUID(IDA);
			Entity b = MonoScriptEngine::GetSceneContext()->GetEntityByUUID(IDB);

			RR_CORE_INFO("{} collided with {}", a.GetName(), b.GetName());
		}
	}

	void ContactListener2D::EndContact(b2Contact* contact)
	{
		if (MonoScriptEngine::GetSceneContext() != nullptr) {
			UUID IDA = (UUID)contact->GetFixtureA()->GetBody()->GetUserData().pointer;
			UUID IDB = (UUID)contact->GetFixtureB()->GetBody()->GetUserData().pointer;

			Entity a = MonoScriptEngine::GetSceneContext()->GetEntityByUUID(IDA);
			Entity b = MonoScriptEngine::GetSceneContext()->GetEntityByUUID(IDB);

			RR_CORE_INFO("{} stopped colliding with {}", a.GetName(), b.GetName());
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

