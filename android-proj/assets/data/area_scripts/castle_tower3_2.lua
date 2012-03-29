music = "castle.caf"

function start()
	down = Portal:new{x=1, y=5, width=1, height=1}
	chest0 = Chest:new{x=6, y=6, milestone=MS_CASTLE_TOWER3_CHEST1, index=ITEM_ELIXIR, quantity=1}
end

function stop()
end

function update(step)
	down:update()

	if (down.go_time) then
		change_areas("castle_tower3_1", 1, 5, DIRECTION_EAST)
	end
end

function activate(activator, activated)
	if (activated == chest0.id) then
		chest0:activate()
	end
end

function collide(id1, id2)
end

