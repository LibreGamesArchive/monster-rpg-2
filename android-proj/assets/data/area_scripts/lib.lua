if (getMilestone(MS_KILLED_GOLEMS)) then
	music = "Flowey.caf"
end

function start()
	out = Portal:new{x=7, y=10}
	down = Portal:new{x=7, y=3}

	girl = Object:new{x=7, y=5, anim_set="geisha", person=true, move_type=MOVE_WANDER}
end

function stop()
end

function update(step)
	if (out:update()) then
		change_areas("flowey", 13, 19, DIRECTION_SOUTH)
	elseif (down:update()) then
		change_areas("libb", 7, 3, DIRECTION_SOUTH)
	end

	girl:move(step)
end

function activate(activator, activated)
	if (activated == girl.id) then
		if (getMilestone(MS_BEAT_TODE)) then
			doDialogue("Your friend finished his work here...\n")
		elseif (getMilestone(MS_BEACH_BATTLE_DONE)) then
			doDialogue("My father is helping your friend downstairs... He has so much knowledge, oh my!\n")
		else
			doDialogue("One day I will manage this library just as my father has for so many years...\n")
		end
	end
end

function collide(id1, id2)
end

