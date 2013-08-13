music = "castle.ogg"

function start()
	down = Portal:new{x=7, y=5, width=1, height=1}
	chest0 = Chest:new{x=1, y=4, milestone=MS_CASTLE_TOWER1_CHEST0, itemtype=ITEM_GOLD, quantity=100}
	if (not getMilestone(MS_BEACH_BATTLE_DONE)) then
		faelon = Object:new{x=4, y=3, anim_set="Faelon", direction=DIRECTION_SOUTH, person=true}
	end
end

function stop()
end

function update(step)
	down:update()

	if (down.go_time) then
		change_areas("castle_tower1_2", 7, 5, DIRECTION_WEST)
	end
end

function activate(activator, activated)
	if (activated == chest0.id) then
		chest0:activate()
	elseif (not (faelon == nil) and activated == faelon.id) then
		doDialogue("Faelon: Please, give me some privacy...\n", true)
	end
end

function collide(id1, id2)
end

