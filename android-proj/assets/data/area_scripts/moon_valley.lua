music="moon.caf"

enemies = {}
enemies[0] = "4UFOs"
enemies[1] = "2Glaces"
enemies[2] = "1Sevalecan"
enemies[3] = "3BigCheeses"
enemies[4] = "1BigCheese2Peepers"
enemies[5] = "2UFOs1Planet"
enemies[6] = "2Macrocats"
enemies[7] = "3Grinners"
enemies[8] = "5Husks"

exceptions = {
	{ 14, 199 },
	{ 15, 199 },
	{ 14, 0 },
	{ 15, 0 },
}

function start()
	chest1 = Chest:new{x=3, y=69, anim_set="chest", milestone=MS_MOON_VALLEY_CHEST_1, index=ITEM_ALIEN_ROBE}
	chest2 = Chest:new{x=7, y=162, anim_set="chest", milestone=MS_MOON_VALLEY_CHEST_2, index=ITEM_ALIEN_SWORD}
	chest3 = Chest:new{x=24, y=162, anim_set="chest", milestone=MS_MOON_VALLEY_CHEST_3, index=ITEM_ALIEN_ROBE}
	chest4 = Chest:new{x=26, y=101, anim_set="chest", milestone=MS_MOON_VALLEY_CHEST_4, index=ITEM_ALIEN_CAP}

	out1 = Portal:new{x=14, y=0, width=2, height=1}
	out2 = Portal:new{x=14, y=199, width=2, height=1}
end

function stop()
end

function update(step)
	if (out1:update()) then
		setMilestone(MS_PAST_MOON_VALLEY, true)
		doMap("valley", "map2")
	elseif (out2:update()) then
		doMap("valley", "map2")
	end

	check_battle(30, enemies, exceptions)
end

function activate(activator, activated)
	if (activated == chest1.id) then
		chest1:activate()
	elseif (activated == chest2.id) then
		chest2:activate()
	elseif (activated == chest3.id) then
		chest3:activate()
	elseif (activated == chest4.id) then
		chest4:activate()
	end
end

function collide(id1, id2)
end

