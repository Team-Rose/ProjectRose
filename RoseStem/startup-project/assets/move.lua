speed = 4
rotspeed = 2

vel = Vec2.new()
ang = 0

rself:SetGravityScale(0)
log(Key.left)

function Update(ts)
	if Input.IsKeyPressed(Key.right) then
		vel.x = vel.x + speed
	end
	if Input.IsKeyPressed(Key.left) then
		vel.x = vel.x - speed
	end
	if Input.IsKeyPressed(Key.up) then
		vel.y = vel.y + speed
	end
	if Input.IsKeyPressed(Key.down) then
		vel.y = vel.y - speed
	end
	if Input.IsKeyPressed(Key.e) then
		ang = ang - (rotspeed*ts)
	end
	if Input.IsKeyPressed(Key.q) then
		ang = ang + (rotspeed*ts)
	end

	rself:SetLinearVelocity(vel)
	rself:SetAnglearVelocity(0)
	rself.rotation.z = ang
	vel = Vec2.new()
end