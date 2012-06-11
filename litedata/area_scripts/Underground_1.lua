music = "underground.ogg"

enemies = {}
enemies[0] = "2Wolves"
enemies[1] = "1Fiend2Leechs"
enemies[2] = "3Weepers"
enemies[3] = "1Spider"

exceptions = {
	{ 7, 34 },
	{ 35, 34 },
};

function start()
	chest1 = Chest:new{x=5, y=5, anim_set="chest", milestone=MS_UNDERGROUND_1_CHEST_0, index=ITEM_CURE}
	chest2 = Chest:new{x=6, y=5, anim_set="chest", milestone=MS_UNDERGROUND_1_CHEST_1, index=ITEM_HOLY_WATER}
	chest3 = Chest:new{x=7, y=5, anim_set="chest", milestone=MS_UNDERGROUND_1_CHEST_2, index=ITEM_CURE}
	portal1 = Portal:new{x=7, y=34}
	portal2 = Portal:new{x=35, y=34}
end

function stop()
end

function update(step)
	check_battle(30, enemies, exceptions)

	portal1:update()
	portal2:update()
	
	if (portal1.go_time) then
		change_areas("Underground_0", 50, 21, DIRECTION_SOUTH);
	elseif (portal2.go_time) then
		change_areas("Underground_2", 3, 8, DIRECTION_SOUTH);
	end
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

