music = "fortress.ogg"

exceptions = {
	{ 1, 6 },
	{ 6, 4 },
}

function start()
	up = Portal:new{x=1, y=6}
	down = Portal:new{x=6, y=4}
end

function stop()
end

function update(step)
	if (up:update()) then
		change_areas("fort2-3", 1, 6, DIRECTION_NORTH)
	elseif (down:update()) then
		change_areas("fort_start", 65, 40, DIRECTION_SOUTH)
	end
	
	check_battle(25, fort_enemies, exceptions)
end

function activate(activator, activated)
end

function collide(id1, id2)
end

