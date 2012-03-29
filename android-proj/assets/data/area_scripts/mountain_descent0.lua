music = "mountains.caf"

function start()
	portal_out = Portal:new{x=3, y=6}
	portal_down = Portal:new{x=5, y=4}
end

function stop()
end

function update(step)
	portal_out:update()
	portal_down:update()

	if (portal_out.go_time) then
		change_areas("mountains", 111, 15, DIRECTION_SOUTH)
	elseif (portal_down.go_time) then
		change_areas("mountain_descent1", 5, 4, DIRECTION_WEST)
	end
end

function activate(activator, activated)
end

function collide(id1, id2)
end

