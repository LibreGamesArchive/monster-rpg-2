music = "volcano.caf"

function start()
	out = Portal:new{x=7, y=9}
	chest = Chest:new{x=7, y=5, anim_set="chest", milestone=MS_VOLCANO_SAVE_CHEST, index=ITEM_ELIXIR, quantity=3}
end

function stop()
end

function update(step)
	if (out:update()) then
		change_areas("volcano", 55, 15, DIRECTION_SOUTH)
	end
end

function activate(activator, activated)
	if (activated == chest.id) then
		chest:activate()
	end
end

function collide(id1, id2)
end

