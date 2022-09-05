#include "rrpch.h"
#include "LuaTypes.h"
#include <box2d/b2_body.h>


namespace Rose {
	
	void LuaRigidBody2D::SetLinearVelocity(Vec2 vel)
	{
		if (entity->HasComponent<Rigidbody2DComponent>()) {
			auto& rb2d = entity->GetComponent<Rigidbody2DComponent>();

			b2Body* body = (b2Body*)rb2d.RuntimeBody;
			body->SetLinearVelocity({ vel.x, vel.y });
		}
	}

	void LuaRigidBody2D::SetTransform(Vec2 pos, float rotation)
	{
		if (entity->HasComponent<Rigidbody2DComponent>()) {
			auto& rb2d = entity->GetComponent<Rigidbody2DComponent>();

			b2Body* body = (b2Body*)rb2d.RuntimeBody;
			body->SetTransform({ pos.x, pos.y }, rotation);
			body->SetAwake(true);
		}
	}

	Vec2 LuaRigidBody2D::GetLinearVelocity()
	{
		if (entity->HasComponent<Rigidbody2DComponent>()) {
			auto& rb2d = entity->GetComponent<Rigidbody2DComponent>();

			b2Body* body = (b2Body*)rb2d.RuntimeBody;
			return Vec2(body->GetLinearVelocity().x, body->GetLinearVelocity().y);
		}
		return Vec2();
	}

	void LuaRigidBody2D::SetAnglearVelocity(float vel)
	{
		if (entity->HasComponent<Rigidbody2DComponent>()) {
			auto& rb2d = entity->GetComponent<Rigidbody2DComponent>();

			b2Body* body = (b2Body*)rb2d.RuntimeBody;
			body->SetAngularVelocity(vel);
		}
	}
	float LuaRigidBody2D::GetAnglearVelocity()
	{
		if (entity->HasComponent<Rigidbody2DComponent>()) {
			auto& rb2d = entity->GetComponent<Rigidbody2DComponent>();

			b2Body* body = (b2Body*)rb2d.RuntimeBody;
			return body->GetAngularVelocity();
		}
	}

	void LuaRigidBody2D::SetGravityScale(float scale)
	{
		if (entity->HasComponent<Rigidbody2DComponent>()) {
			auto& rb2d = entity->GetComponent<Rigidbody2DComponent>();

			b2Body* body = (b2Body*)rb2d.RuntimeBody;
			body->SetGravityScale(scale);
		}
	}
	float LuaRigidBody2D::GetGravityScale()
	{
		if (entity->HasComponent<Rigidbody2DComponent>()) {
			auto& rb2d = entity->GetComponent<Rigidbody2DComponent>();

			b2Body* body = (b2Body*)rb2d.RuntimeBody;
			return body->GetGravityScale();
		}
		return 0.0f;
	}
	void LuaEntity::SyncToLua() {
		{entity.GetComponent<TagComponent>().Tag = Name; }
		{
			TransformComponent tc = entity.GetComponent<TransformComponent>();
			tc.Translation = { position.x , position.y, position.z };
			tc.Rotation = { rotation.x, rotation.y, rotation.z };
			tc.Scale = { size.x, size.y, size.z };
		}
	}
}