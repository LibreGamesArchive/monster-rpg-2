music = "shyzu.ogg"

function start()
	out = Portal:new{x=2, y=6}
	female = Object:new{x=1, y=3, anim_set="shyzu_female", person=true}
	setObjectDirection(female.id, DIRECTION_SOUTH)
	male = Object:new{x=3, y=3, anim_set="shyzu_male", person=true}
	setObjectDirection(male.id, DIRECTION_SOUTH)
	chest = Chest:new{x=6, y=2, milestone=MS_SHYZU_CHEST_1, itemtype=ITEM_GOLD, quantity=500}
end

function stop()
end

function update(step)
	if (out:update()) then
		change_areas("shyzu", 8, 8, DIRECTION_SOUTH)
	end
end

function activate(activator, activated)
	if (activated == female.id) then
		setObjectDirection(female.id, player_dir(female))
		doDialogue("Female: Oh, Earthlings, how wonderful!\n", true)
		setObjectDirection(female.id, DIRECTION_SOUTH)
	elseif (activated == male.id) then
		setObjectDirection(male.id, player_dir(male))
		doDialogue("Male: We don't get many of your type around here these days...\n", true)
		setObjectDirection(male.id, DIRECTION_SOUTH)
	elseif (activated == chest.id) then
		chest:activate()
	end
end

function collide(id1, id2)
end

