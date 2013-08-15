music = "underground.ogg"

function start()
	chest0 = Chest:new{x=2, y=3, anim_set="chest", milestone=MS_UNDERGROUND_CHEST_0, index=ITEM_CURE, quantity=3}
	chest1 = Chest:new{x=3, y=3, anim_set="chest", milestone=MS_UNDERGROUND_CHEST_1, index=ITEM_BONE_BLADE}
	chest2 = Chest:new{x=4, y=3, anim_set="chest", milestone=MS_UNDERGROUND_CHEST_2, index=ITEM_LETTER}
	chest3 = Chest:new{x=5, y=3, anim_set="chest", milestone=MS_UNDERGROUND_CHEST_3, index=ITEM_BONE_MAIL}
	chest4 = Chest:new{x=6, y=3, anim_set="chest", milestone=MS_UNDERGROUND_CHEST_4, index=ITEM_CURE, quantity=3}
	portal_up = Portal:new{x=1, y=5}
	portal_out = Portal:new{x=4, y=9}
end

function stop()
end

function update(step)
	portal_up:update()
	portal_out:update()

	if (portal_up.go_time) then
		change_areas("Keep_cells", 14, 3, DIRECTION_SOUTH);
	end
	if (portal_out.go_time) then
		change_areas("Underground_0", 3, 4, DIRECTION_SOUTH);
	end
end

function activate(activator, activated)
	if (activated == chest0.id) then
		chest0:activate()
	elseif (activated == chest1.id) then
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

