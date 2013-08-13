if (getMilestone(MS_KILLED_GOLEMS)) then
	music = "Flowey.ogg"
end

function start()
	down = Portal:new{x=11, y=7}
	chest = Chest:new{x=1, y=3, anim_set="chest", milestone=MS_GUARDS_CHEST, index=ITEM_GUARD_SWORD}

	if (getMilestone(MS_GUARDS_LEFT_BEACH)) then
		guards = {}
		for i=0,4 do
			guards[i+1] = Object:new{x=2+(i*2), y=3, anim_set="Guard", person=true, direction=DIRECTION_SOUTH}
			setObjectSubAnimation(guards[i+1].id, "sleep")
		end
		updateArea()
		for i=1,5 do
			setObjectSubAnimation(guards[i].id, "sleep")
		end
	end
end

function stop()
end

function update(step)
	if (down:update()) then
		change_areas("flowey_barracks", 11, 7, DIRECTION_WEST)
	end
end

function activate(activator, activated)
	if (activated == chest.id) then
		chest:activate()
	end
	
	if (getMilestone(MS_GUARDS_LEFT_BEACH)) then
		for i=1,5 do
			if (activated == guards[i].id) then
				doDialogue("Zzz...\n")
			end
		end
	end
end

function collide(id1, id2)
end

