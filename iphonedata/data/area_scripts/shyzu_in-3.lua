music = "shyzu.ogg"

function start()
	out = Portal:new{x=2, y=6}
	female = Object:new{x=2, y=3, anim_set="shyzu_female", person=true, direction=DIRECTION_SOUTH}
end

function stop()
end

function update(step)
	if (out:update()) then
		change_areas("shyzu", 8, 16, DIRECTION_SOUTH)
	end
end

function activate(activator, activated)
	if (activated == female.id) then
		local _d = getObjectDirection(female.id)
		setObjectDirection(female.id, player_dir(female))
		doDialogue("Female: Ages ago a meteor landed where this village now stands...\nFemale: Something in its makeup has kept evil away to this day.\n", true)
		setObjectDirection(female.id, _d)
	end
end

function collide(id1, id2)
end

