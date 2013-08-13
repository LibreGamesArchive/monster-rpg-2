music = "fortress.ogg"

exceptions = {
	{ 5, 7 },
}

function start()
	up = Portal:new{x=5, y=7}
	chest = Chest:new{x=10, y=5, anim_set="chest", milestone=MS_FORT_UNDER_CHEST_2, index=ITEM_CURE3}
end

function stop()
end

function update(step)
	if (up:update()) then
		change_areas("fort_start", 60, 42, DIRECTION_SOUTH)
	end
	
	check_battle(25, fort_enemies, exceptions)
end

function activate(activator, activated)
	if (activated == chest.id) then
		chest:activate()
	end
end

function collide(id1, id2)
end

