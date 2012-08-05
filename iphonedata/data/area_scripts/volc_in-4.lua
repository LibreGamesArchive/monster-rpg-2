music = "volcano.ogg"

enemies = {}
enemies[0] = "1Efreet"
enemies[1] = "1FireAnt"
enemies[2] = "2Lavas2Vultures"
enemies[3] = "1BigBlue"
enemies[4] = "1Kromagma1Flamer1Zombie"
enemies[5] = "4Zombies"
enemies[6] = "1Flamer2Magmers"
enemies[7] = "2Flamers"
enemies[8] = "2Lavas"
enemies[9] = "2Magmods"


exceptions = {
	{ 6, 24 },
	{ 6, 29 },
}

function start()
	down = Portal:new{x=6, y=24}
	out = Portal:new{x=6, y=29}
end

function stop()
end

function update(step)
	if (down:update()) then
		change_areas("volc_in-3", 6, 24, DIRECTION_SOUTH)
	elseif (out:update()) then
		change_areas("volcano", 16, 17, DIRECTION_SOUTH)
	end

	check_battle(35, enemies, exceptions)
end

function activate(activator, activated)
end

function collide(id1, id2)
end

