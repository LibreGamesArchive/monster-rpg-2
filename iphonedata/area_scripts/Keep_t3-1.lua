music = "keep.ogg"

enemies = {}
enemies[0] = "3Stabbers"
enemies[1] = "2Stabbers2Halberds"
enemies[2] = "1Beast"
enemies[3] = "3Rages"

exceptions = {
	{3,4},
	{4,4},
}

function start()
	up = Portal:new{x=4, y=4}
	down = Portal:new{x=3, y=4}
end

function stop()
end

function update(step)
	check_battle(30, enemies, exceptions)

	if (up:update()) then
		change_areas("Keep_t3-2", 4, 4, DIRECTION_WEST)
	elseif (down:update()) then
		change_areas("Keep_4", 3, 18, DIRECTION_EAST)
	end
end

function activate(activator, activated)
end

function collide(id1, id2)
end

