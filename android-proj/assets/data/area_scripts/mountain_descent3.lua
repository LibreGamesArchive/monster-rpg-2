music = "mountains.ogg"

function start()
	portal_out = Portal:new{x=3, y=6}
	portal_up = Portal:new{x=5, y=4}
end

function stop()
end

function update(step)
	if (not getMilestone(MS_UNLOCKED_FLOWEY)) then
		setMilestone(MS_UNLOCKED_FLOWEY, true)
	end

	portal_out:update()
	portal_up:update()

	if (portal_out.go_time) then
		stopObject(0)
		doMap("mountains")
	elseif (portal_up.go_time) then
		change_areas("mountain_descent2", 5, 4, DIRECTION_WEST)
	end
end

function activate(activator, activated)
end

function collide(id1, id2)
end

