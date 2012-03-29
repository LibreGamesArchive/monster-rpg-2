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
	{ 20, 29 },
}


function start()
	out = Portal:new{x=20, y=29}
	
	chest1 = Chest:new{x=4, y=27, anim_set="blue_chest", milestone=MS_VOLCANO_CHEST_2, index=ITEM_HEAL}
	chest2 = Chest:new{x=69, y=24, anim_set="blue_chest", milestone=MS_VOLCANO_CHEST_3, index=ITEM_CURE3, quantity=5}
	chest3 = Chest:new{x=73, y=26, anim_set="blue_chest", milestone=MS_VOLCANO_CHEST_4, index=ITEM_ELIXIR}
end

function stop()
end

function update(step)
	if (out:update()) then
		change_areas("volcano", 34, 38, DIRECTION_SOUTH)
	end

	check_battle(35, enemies, exceptions)
end

function activate(activator, activated)
	if (activated == chest1.id) then
		chest1:activate()
	elseif (activated == chest2.id) then
		chest2:activate()
	elseif (activated == chest3.id) then
		chest3:activate()
	end
end

function collide(id1, id2)
end

