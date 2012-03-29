music = "castle.caf"

function start()
	down = Portal:new{x=7, y=5, width=1, height=1}
	astronomer = Object:new{x=2, y=4, anim_set="astronomer", person=true}
	setObjectDirection(astronomer.id, DIRECTION_WEST)
end

function stop()
end

function update(step)
	down:update()

	if (down.go_time) then
		change_areas("castle_tower2_2", 7, 5, DIRECTION_WEST)
	end
end

function activate(activator, activated)
	if (activated == astronomer.id) then
		setObjectDirection(astronomer.id, player_dir(astronomer))
		if (getMilestone(MS_BEAT_TODE)) then
			doDialogue("Astronomer: Where will this planet take us next, hoohoo!\n", true)
		elseif (getMilestone(MS_BEAT_RIDER)) then
			doDialogue("Astronomer: Oh dear, the volcano is being active... but I see the moon is a little bit brighter too...\n", true)
		else
			doDialogue("Astronomer: Ohhh... you must be part of the disturbance in the stars!\nAstronomer: Yes, the moons are lining up and new stars are appearing all the time...\nAstronomer: I see a mountain, and a floating fortress... but that is not the end, oh no! Hehehe...\n", true)
		end
		setObjectDirection(astronomer.id, DIRECTION_WEST)
	end
end

function collide(id1, id2)
end

