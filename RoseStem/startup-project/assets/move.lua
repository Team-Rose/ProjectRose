speed = 4
rotspeed = 2

vel = Vec2.new()
ang = 0

rself:GetRigidBody2D():SetGravityScale(0)
log(Key.left)

function Update(ts)
	if Input.IsKeyPressed(Key.d) then
		vel.x = vel.x + speed
	end
	if Input.IsKeyPressed(Key.a) then
		vel.x = vel.x - speed
	end
	if Input.IsKeyPressed(Key.w) then
		vel.y = vel.y + speed
	end
	if Input.IsKeyPressed(Key.s) then
		vel.y = vel.y - speed
	end
	if Input.IsKeyPressed(Key.e) then
		ang = ang - (rotspeed*ts)
	end
	if Input.IsKeyPressed(Key.q) then
		ang = ang + (rotspeed*ts)
	end

	rself:GetRigidBody2D():SetLinearVelocity(vel)
	rself:GetRigidBody2D():SetAnglearVelocity(0)
	rself.rotation.z = ang
	vel = Vec2.new()
end