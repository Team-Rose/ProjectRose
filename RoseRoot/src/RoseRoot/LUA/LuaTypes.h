#pragma once
#include "RoseRoot/Scene/Components.h"
#include <RoseRoot/Scene/Entity.h>

namespace Rose {
	struct Vec2 {
		float x = 0, y = 0;

		Vec2() {}
		Vec2(float lx, float ly)
			: x(lx), y(ly) {}
		~Vec2() {}
	};
	struct Vec3 {
		float x = 0, y = 0, z = 0;

		Vec3() {}
		Vec3(float lx, float ly, float lz)
			: x(lx), y(ly), z(lz) {}
		~Vec3() {}
	};
	struct Color {
		float r = 0, g = 0, b = 0, a = 1;

		Color() {}
		Color(float r, float g, float b, float a)
			: r(r), g(g), b(b), a(a) {}
		~Color() {}
	};

	struct LuaEntity {
		Entity entity;

		std::string Name = "entity";
		Vec3 position;
		Vec3 rotation;
		Vec3 size;

		LuaEntity(Entity ent) { entity = ent; SyncToRose(); }

		void SetLinearVelocity(Vec2 vel);
		Vec2 GetLinearVelocity();

		void SetAnglearVelocity(float vel);
		float GetAnglearVelocity();

		void SetGravityScale(float scale);
		float GetGravityScale();

		void SyncToRose() {
			{Name = entity.GetComponent<TagComponent>().Tag; }
			{
				TransformComponent tc = entity.GetComponent<TransformComponent>();
				glm::vec3 gPos = tc.Translation;
				position = Vec3(gPos.x, gPos.y, gPos.z);

				glm::vec3 gRot = tc.Rotation;
				rotation = Vec3(gRot.x, gRot.y, gRot.z);

				glm::vec3 gSize = tc.Scale;
				size = Vec3(gSize.x, gSize.y, gSize.z);
			}

		}

		void SyncToLua();
	};

}