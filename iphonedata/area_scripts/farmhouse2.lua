music = "farmer.ogg"

function start()
	down = Portal:new{x=1, y=5}

	add_downward_light(54, 28, 8, 40, 60, 255, 255, 200, 42)
	add_downward_light(86, 28, 8, 40, 60, 255, 255, 200, 42)
	add_downward_light(140, 28, 8, 40, 60, 255, 255, 200, 42)
	add_downward_light(172, 28, 8, 40, 60, 255, 255, 200, 42)

	big_momma = Object:new{x=9, y=6, anim_set="big_momma", person=true, move_type=MOVE_WANDER}
end

function stop()
end

function update(step)
	if (down:update()) then
		change_areas("farmhouse", 1, 5, DIRECTION_EAST)
	end

	big_momma:move(step)
end

function activate(activator, activated)
	if (activated == big_momma.id) then
		setObjectDirection(big_momma.id, player_dir(big_momma))
		doDialogue("It's great to be home! The monsters only ate 1 of my babies... cows that is.\n")
	end
end

function collide(id1, id2)
end

