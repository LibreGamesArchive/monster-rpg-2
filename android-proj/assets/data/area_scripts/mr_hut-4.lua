music = "Muttrace.ogg"

function start()
	out = Portal:new{x=3, y=7}
	chest1 = Chest:new{x=2, y=2, anim_set="chest", milestone=MS_MUTTRACE_CHEST_1, index=ITEM_CURE3}
	chest2 = Chest:new{x=4, y=2, anim_set="chest", milestone=MS_MUTTRACE_CHEST_2, index=ITEM_ELIXIR}
	chest3 = Chest:new{x=1, y=3, anim_set="chest", milestone=MS_MUTTRACE_CHEST_3, index=ITEM_HOLY_WATER}
	chest4 = Chest:new{x=5, y=3, anim_set="chest", milestone=MS_MUTTRACE_CHEST_4, itemtype=ITEM_GOLD, quantity=500}
end

function stop()
end

function update(step)
	if (out:update()) then
		change_areas("Muttrace", 33, 9, DIRECTION_SOUTH)
	end
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

