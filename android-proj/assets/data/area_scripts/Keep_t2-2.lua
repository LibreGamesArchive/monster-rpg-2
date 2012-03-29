music = "keep.caf"

enemies = {}
enemies[0] = "3Stabbers"
enemies[1] = "2Stabbers2Halberds"
enemies[2] = "1Beast"
enemies[3] = "3Rages"

exceptions = {
	{4,4}
}

function start()
	down = Portal:new{x=4, y=4}
	chest = Chest:new{x=3, y=3, anim_set="chest", milestone=MS_KEEP_TOWER2_CHEST, index=ITEM_MYSTIC_MAIL}
end

function stop()
end

function update(step)
	check_battle(30, enemies, exceptions)

	if (down:update()) then
		change_areas("Keep_t2-1", 4, 4, DIRECTION_WEST)
	end
end

function activate(activator, activated)
	if (activated == chest.id) then
		chest:activate()
	end
end

function collide(id1, id2)
end

