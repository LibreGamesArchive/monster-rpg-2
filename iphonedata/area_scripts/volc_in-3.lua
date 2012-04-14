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
	{ 4, 26 },
}

function start()
	up = Portal:new{x=6, y=24}
	down = Portal:new{x=4, y=26}
	
	chest1 = Chest:new{x=76, y=24, anim_set="blue_chest", milestone=MS_VOLCANO_CHEST_6, index=ITEM_ELIXIR, quantity=3}
	chest2 = Chest:new{x=76, y=25, anim_set="blue_chest", milestone=MS_VOLCANO_CHEST_7, index=ITEM_CURE3}
end

function stop()
end

function update(step)
	if (up:update()) then
		change_areas("volc_in-4", 6, 24, DIRECTION_SOUTH)
	elseif (down:update()) then
		change_areas("volc_in-2", 4, 26, DIRECTION_EAST)
	end

	check_battle(35, enemies, exceptions)
end

function activate(activator, activated)
	if (activated == chest1.id) then
		chest1:activate()
	elseif (activated == chest2.id) then
		chest2:activate()
	end
end

function collide(id1, id2)
end

