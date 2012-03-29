music = "castle.caf"

function start()
	down = Portal:new{x=1, y=5, width=1, height=1}
	up = Portal:new{x=7, y=5, width=1, height=1}
end

function stop()
end

function update(step)
	up:update()
	down:update()

	if (down.go_time) then
		change_areas("castle_tower1_1", 1, 5, DIRECTION_EAST)
	end
	if (up.go_time) then
		change_areas("castle_tower1_3", 7, 5, DIRECTION_WEST)
	end
end

function activate(activator, activated)
end

function collide(id1, id2)
end

