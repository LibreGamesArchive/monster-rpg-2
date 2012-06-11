music = "castle.ogg"

function start()
	exit = Portal:new{x=4, y=9, width=1, height=1}
	up = Portal:new{x=1, y=5, width=1, height=1}
end

function stop()
end

function update(step)
	exit:update()
	up:update()

	if (exit.go_time) then
		change_areas("castle", 36, 34, DIRECTION_SOUTH)
	end
	if (up.go_time) then
		change_areas("castle_tower2_2", 1, 5, DIRECTION_EAST)
	end
end

function activate(activator, activated)
end

function collide(id1, id2)
end

