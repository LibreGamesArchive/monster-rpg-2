music = "jungle_shop.ogg"

function start()
	out = Portal:new{x=3, y=7}
	chest = Chest:new{x=4, y=4, anim_set="chest", milestone=MS_JUNGLE_JUICE_CHEST, index=ITEM_JUICE}
end

function stop()
end

function update(step)
	if (out:update()) then
		change_areas("jungle_village", 7, 14, DIRECTION_SOUTH);
	end
end

function activate(activator, activated)
	if (activated == chest.id) then
		chest:activate()
	end
end

function collide(id1, id2)
end

