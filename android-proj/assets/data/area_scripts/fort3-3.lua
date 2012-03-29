music = "fortress.caf"

exceptions = {
	{ 1, 6 },
	{ 6, 4 },
}

function start()
	down = Portal:new{x=1, y=6}
	chest = Chest:new{x=4, y=4, anim_set="chest", battle="1Carrot", milestone=MS_CHEST_FLINT3}
end

function stop()
end

function update(step)
	if (down:update()) then
		change_areas("fort3-2", 1, 6, DIRECTION_NORTH)
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

