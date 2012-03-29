music = "volcano.caf"

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
	{ 4, 26 },
	{ 73, 29 },
}

function start()
	out = Portal:new{x=73, y=29}
	up = Portal:new{x=4, y=26}
	
	chest1 = Chest:new{x=29, y=20, anim_set="blue_chest", milestone=MS_VOLCANO_CHEST_5, index=ITEM_HOLY_WATER}
end

function stop()
end

function update(step)
	if (out:update()) then
		change_areas("volcano", 61, 39, DIRECTION_SOUTH)
	elseif (up:update()) then
		change_areas("volc_in-3", 4, 26, DIRECTION_EAST)
	end

	check_battle(35, enemies, exceptions)
end

function activate(activator, activated)
	if (activated == chest1.id) then
		chest1:activate()
	end
end

function collide(id1, id2)
end

