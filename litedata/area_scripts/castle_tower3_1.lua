music = "castle.ogg"

function start()
	exit = Portal:new{x=4, y=9, width=1, height=1}
	up = Portal:new{x=1, y=5, width=1, height=1}
	chest0 = Chest:new{x=6, y=6, anim_set="chest", milestone=MS_CASTLE_TOWER3_CHEST0, index=ITEM_HOLY_WATER, quantity=1}
end

function stop()
end

function update(step)
	exit:update()
	up:update()

	if (exit.go_time) then
		change_areas("castle", 15, 44, DIRECTION_SOUTH)
	end
	if (up.go_time) then
		change_areas("castle_tower3_2", 1, 5, DIRECTION_EAST)
	end
end

function activate(activator, activated)
	if (activated == chest0.id) then
		chest0:activate()
	end
end

function collide(id1, id2)
end

