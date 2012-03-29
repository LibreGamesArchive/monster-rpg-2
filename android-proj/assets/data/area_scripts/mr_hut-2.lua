music = "Muttrace.caf"

function start()
	out = Portal:new{x=3, y=7}
	chest = Chest:new{x=2, y=2, milestone=MS_MUTTRACE_ELIXIR, index=ITEM_ELIXIR}
	setObjectDimensions(chest.id, 32, 16)
end

function stop()
end

function update(step)
	if (out:update()) then
		change_areas("Muttrace", 8, 9, DIRECTION_SOUTH)
	end
end

function activate(activator, activated)
	if (activated == chest.id) then
		chest:activate()
	end
end

function collide(id1, id2)
end

