if (getMilestone(MS_KILLED_GOLEMS)) then
	music = "Flowey.ogg"
end

function start()
	out = Portal:new{x=6, y=11}
	bartender = Object:new{x=6, y=4, anim_set="bartender", person=true}
	bart2 = Object:new{x=6, y=5}
	sally = Object:new{x=1, y=8, anim_set="SallyJones", person=true}
	setObjectDirection(sally.id, DIRECTION_EAST)

	d0 = Object:new{x=5, y=6, anim_set="drunkard0", person=true}
	d1 = Object:new{x=7, y=6, anim_set="drunkard1", person=true}
	setObjectDirection(d0.id, DIRECTION_NORTH)
	setObjectDirection(d1.id, DIRECTION_NORTH)

	door = Object:new{x=6, y=2, width=1, height=1}
end

function stop()
end

function update(step)
	if (out:update()) then
		change_areas("flowey", 22, 21, DIRECTION_SOUTH)
	end
end

function activate_bartender()
	if (getMilestone(MS_TAVERN_JUICE)) then
		doDialogue("Bartender: Noooooo way mister, you're cut off!\n")
	else
		slot = findEmptyInventorySlot()
		if (slot < 0) then
			doDialogue("Bartender: * whistles *\n")
		else
			doDialogue("Bartender: One swig of this will put hair on your chest... er... Mam.\n", true)
			setInventory(slot, ITEM_JUICE, 1)
			loadPlayDestroy("chest.ogg")
			setMilestone(MS_TAVERN_JUICE, true);
		end
	end
end

function activate(activator, activated)
	if (activated == bartender.id or activated == bart2.id) then
		activate_bartender()
	elseif (activated == sally.id) then
		setObjectDirection(sally.id, player_dir(sally))
		if (not getMilestone(MS_GOT_MEDALLION) and getMilestone(MS_GOT_KEY)) then
			doDialogue("Oh, my key, you found it! Here, take this as a token of my appreciation!\n", true)
			slot = findUnfullInventorySlot(ITEM_KEY)
			setInventory(slot, ITEM_MEDALLION, 1)
			loadPlayDestroy("chest.ogg")
			setMilestone(MS_GOT_MEDALLION, true)
			doDialogue(_t("Got a ") .. getItemIcon(ITEM_MEDALLION) .. _t("Medallion") .. "\n", true)
		elseif (not getMilestone(MS_GOT_KEY)) then
			doDialogue("Oh my dear, I've lost my key... It's about this big and fancy...\nIf you find it, please, bring it to me!\n", true)
		else
			doDialogue("Thank you so much for finding my key!\n", true)
		end
		setObjectDirection(sally.id, DIRECTION_EAST)
	elseif (activated == d0.id) then
		setObjectDirection(d0.id, player_dir(d0))
		doDialogue("Hi, I'm Leonard...\n", true)
		setObjectDirection(d0.id, DIRECTION_NORTH)
	elseif (activated == d1.id) then
		setObjectDirection(d1.id, player_dir(d1))
		doDialogue("Hi, I'm Carleson...\n", true)
		setObjectDirection(d1.id, DIRECTION_NORTH)
	elseif (activated == door.id) then
		doDialogue("You can't go in there...\n")
	end
end

function activate_any(tx, ty)
	local px, py = getObjectPosition(0)
	if (px == 6 and py == 6) then
		if (tx == 6 and ty == 4) then
			stopObject(0)
			setObjectDirection(0, DIRECTION_NORTH)
			activate_bartender()
			return true
		end
	end
	return false
end

function collide(id1, id2)
end

