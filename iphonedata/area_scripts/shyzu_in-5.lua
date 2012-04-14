music = "shyzu.ogg"

function start()
	out = Portal:new{x=2, y=6}
	female = Object:new{x=3, y=4, anim_set="shyzu_female", person=true, move_type=MOVE_WANDER}
end

function stop()
end

function update(step)
	if (out:update()) then
		change_areas("shyzu", 28, 24, DIRECTION_SOUTH)
	end

	female:move(step)
end

function activate(activator, activated)
	if (activated == female.id) then
		dir = getObjectDirection(female.id)
		setObjectDirection(female.id, player_dir(female))
		doDialogue("Female: My husband is a very adventurous fellow... he loves it but it worries me.\n", true)
		setObjectDirection(female.dir, dir)
	end
end

function collide(id1, id2)
end

