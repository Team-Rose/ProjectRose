pos = Vec3.new()

--rself is how you access the entity the script is attached to!
pos = rself.position

--Called Before the first frame
function Init()
	log(rself.name)
end

--Called Every Frame
function Update(ts)
	pos.x = pos.x + (ts*0.5)
	rself.position = pos
    if pos.x > 14 then
        pos.x = -14
    end
end
