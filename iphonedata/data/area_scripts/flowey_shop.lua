if (getMilestone(MS_KILLED_GOLEMS)) then
	music = "Flowey.ogg"
end

function start()
	sk0 = Object:new{x=3, y=3, anim_set="shopkeep0"}
	sk0d = Object:new{x=3, y=4}

	sk1 = Object:new{x=19, y=3, anim_set="shopkeep1"}
	sk1d = Object:new{x=19, y=4}

	sign = Object:new{x=11, y=4}

	out = Portal:new{x=11, y=13}
end

function stop()
end

function update(step)
	if (out:update()) then
		change_areas("flowey", 21, 27, DIRECTION_SOUTH)
	end
end

function activate_sk0()
	doDialogue("Shopkeeper: I handle the weapons and armor, my husband deals with potions.\n", true)
	doShop("Shopkeeper", "shopkeep0", 4, ITEM_BAMBOO_STAFF, 50, ITEM_ROUNDS, 2, ITEM_MYSTIC_ROBE, 50, ITEM_MAIL, 75)
end

function activate_sk1()
	doDialogue("Shopkeeper: Hello, welcome to Flowey...\n", true)
	doShop("Shopkeeper", "shopkeep1", 5, ITEM_CURE, 5, ITEM_CURE2, 35, ITEM_HEAL, 30, ITEM_ELIXIR, 200, ITEM_HOLY_WATER, 150)
end

function activate_sign()
	doDialogue("\"...As you can see, we have the lowest prices around...\"\n")
end

function activate(activator, activated)
	if (activated == sk0.id or activated == sk0d.id) then
		activate_sk0()
	elseif (activated == sk1.id or activated == sk1d.id) then
		activate_sk1()
	elseif (activated == sign.id) then
		activate_sign()
	end
end

function activate_any(tx, ty)
	local px, py = getObjectPosition(0)
	if (px == 3 and py == 5) then
		if (tx == 3 and ty == 3) then
			stopObject(0)
			setObjectDirection(0, DIRECTION_NORTH);
			activate_sk0()
			return true
		end
	elseif (px == 19 and py == 5) then
		if (tx == 19 and ty == 3) then
			stopObject(0)
			setObjectDirection(0, DIRECTION_NORTH);
			activate_sk1()
			return true
		end
	elseif (px == 11 and py == 5) then
		if (tx == 11 and ty == 3) then
			setObjectDirection(0, DIRECTION_NORTH);
			activate_sign()
			return true
		end
	end
	return false
end

function collide(id1, id2)
end

