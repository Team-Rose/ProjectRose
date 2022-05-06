#include "rrpch.h"
#include "Lua.h"
#include <string>

#include "RoseRoot/Renderer/Renderer2D.h"
#include "RoseRoot/Scene/Components.h"

namespace Rose {

	

	Vec2 CreateVec2(float x, float y)
	{ return Vec2(x, y);}
	Vec3 CreateVec3(float x, float y, float z)
	{ return Vec3(x, y, z);}

	Color CreateColor(float r, float g, float b, float a)
	{return Color(r, g, b, a);}

	static void DrawQuadSimple(Vec2 pos, Vec2 size, Color color)
	{	//Lua Renderer2D functions will not work with rosestem.
		Renderer2D::DrawQuad({ pos.x, pos.y }, { size.x, size.y }, { color.r, color.g, color.b, color.a });}
	static bool LUAIsKeyDown(Key key) {
		return Input::IsKeyPressed(key);
	}
	static bool LUAIsMouseButtonPressed(Mouse button) {
		return Input::IsMouseButtonPressed(button);
	}
	
	LuaScript::LuaScript(Entity entity, const std::string& filepath)
		:m_LuaEntity(CreateRef<LuaEntity>(LuaEntity(entity)))
	{
		RR_PROFILE_FUNCTION();
		m_LuaState = sol::state();

		m_LuaState.open_libraries(sol::lib::base, sol::lib::package, sol::lib::table);

		//Register Types
		m_LuaState.new_usertype<Vec2>("Vec2","x",&Vec2::x, "y",&Vec2::y);
		m_LuaState.new_usertype<Vec3>("Vec3","x",&Vec3::x, "y",&Vec3::y, "z", &Vec3::z);
		m_LuaState.new_usertype<Color>("Color","r",&Color::r, "g",&Color::g, "b",&Color::b, "a",&Color::a);

		m_LuaState.new_usertype<LuaEntity>( "Entity", 
											"name",&LuaEntity::Name,
											"position",&LuaEntity::position,
											"rotation",&LuaEntity::rotation,
											"size",&LuaEntity::size,
											"SetLinearVelocity", &LuaEntity::SetLinearVelocity,
											"GetLinearVelocity", &LuaEntity::GetLinearVelocity,
											"SetAnglearVelocity", &LuaEntity::SetAnglearVelocity,
											"GetAnglearVelocity", &LuaEntity::GetAnglearVelocity,
											"SetGravityScale", &LuaEntity::SetGravityScale,
											"GetGravityScale", &LuaEntity::GetGravityScale
										  );

		//Register Util/Core Functions
		m_LuaState.set_function("log", &BindRoseLog);

		m_LuaState.set_function("CreateColor", &CreateColor);
		m_LuaState.set_function("CreateVec2", &CreateVec2);
		m_LuaState.set_function("CreateVec3", &CreateVec3);

		//Render2D lua Binding
		auto renderer2D = m_LuaState["Renderer2D"].get_or_create<sol::table>();
		renderer2D.set_function("DrawSimpleQuad", &DrawQuadSimple);

		//Input
		auto key = m_LuaState["Key"].get_or_create<sol::table>();
		for (auto& value : AllKeys) {
			key[KeyToString(value)] = value;
		}
		auto mouse = m_LuaState["Mouse"].get_or_create<sol::table>();
		for (auto& value : AllMouseButtons) {
			mouse[MouseToString(value)] = value;
		}

		auto input = m_LuaState["Input"].get_or_create<sol::table>();
		input.set_function("IsKeyPressed", &LUAIsKeyDown);
		input.set_function("IsMouseButtonPressed", &LUAIsMouseButtonPressed);

		//Globals
		m_LuaState["rself"] = m_LuaEntity;

		int resultL = luaL_loadfile(m_LuaState, filepath.c_str());
		if (resultL == LUA_OK) {
			resultL = lua_pcall(m_LuaState, 0, LUA_MULTRET, 0);
			if (resultL == LUA_OK) {
				m_luaOK = true;
			}else
				RR_CORE_WARN("LUA Script failed to run: {0}", filepath.c_str());
		}else
			RR_CORE_WARN("Failed to lua load file {0}", filepath.c_str());
		
	}

	LuaScript::~LuaScript()
	{
		RR_PROFILE_FUNCTION();
		m_LuaState.end();
	}
	void LuaScript::Init()
	{
		RR_PROFILE_FUNCTION();
		if (!m_luaOK)
			return;
		sol::function Init = m_LuaState["Init"];
		Init();
	}

	void LuaScript::Update(Timestep ts)
	{
		RR_PROFILE_FUNCTION();
		if (!m_luaOK)
			return;

		m_LuaEntity->SyncToRose();
		sol::function Update = m_LuaState["Update"];
		Update((float)ts);
		m_LuaEntity->SyncToLua();
	}

	
	int LuaScript::BindRoseLog(lua_State* ptrState)
	{
		const char* str = lua_tostring(ptrState, 1);
		RR_INFO(str);
		return 0;
	}
}