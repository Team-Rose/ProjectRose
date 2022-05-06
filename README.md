# Project Rose
A easy to use Game engine and Editor!<br>

<img src="https://media.discordapp.net/attachments/918981027538567278/957977029440069662/Rose.png" alt="drawing" width="200"/>

**Features**
-
Easy to learn LUA Scripting (Rose Stem)
```lua
pos = Vec3.new()

--rself is how you access the entity the script is attached to!
pos = rself.position
--use log not print for Rose!
log("Hello from lua")

--Called Before the first frame
function Init()
	log("Init")
end

--Called Every Frame
function Update(ts)
	pos.x = pos.x + (ts*0.5)
	rself.position = pos
    if pos.x > 14 then
        pos.x = -14
    end
    
	log(rself.name..": "..rself.position.x)
end
```

Fast clean C++ API (Rose Root)
```C++
//In your layer update function
RoseRoot::RenderCommand::SetClearColor({0.2, 0.0, 0.2, 1.0});
RoseRoot::RenderCommand::Clear();
RoseRoot::Renderer2D::BeginScene(m_EditorCamera);

//Draws a white quad at the center of the screen
Renderer2D::DrawQuad({ 0.f, 0.f }, { 1.f, 1.f }, { 1.f, 1.f, 1.f, 1.f });

RoseRoot::Renderer2D::EndScene();
```