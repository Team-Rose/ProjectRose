pos = Vec3.new()
size = Vec2.new()
color = CreateColor(1, 1, 0, 1)

size.x = 1
size.y = 1

pos = rself.position
log("Hello from lua")

function Init()
	log("Init")
end

function Update(ts)
	pos.x = pos.x + ts
	rself.position = pos
	log(rself.name..": "..rself.position.x)
end
