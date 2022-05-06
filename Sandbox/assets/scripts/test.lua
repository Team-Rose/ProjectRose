pos = Vec2.new()
size = Vec2.new()
color = CreateColor(1, 1, 0, 1)

size.x = 1
size.y = 1

log("Hello from lua")

function Init()
	log("Init")
end

function Update(ts)
	pos.x = 0;
	Renderer2D.DrawSimpleQuad(pos, size, color)
end
