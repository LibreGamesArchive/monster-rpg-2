if (getMilestone(MS_KILLED_GOLEMS)) then
	music = "Flowey.ogg"
end

count = 0
names = {}

function do_tint()
	set_music_volume(0)
	tint(60, 60, 60, 0)
	loadPlayDestroy("sleep.ogg")
end

function undo_tint()
	reverse_tint(60, 60, 60, 0)
end

function done()
	--print("done 1")
	reviveAllPlayers()
	--print("done 2")
	setObjectSolid(0, true)
	--print("done 3")
	for i=1,count do
		--print("i = ", i)
		if (not (players[i].name == "Eny")) then
			removeObject(players[i].id)
		end
	end
	--print("done 4")
	count = 0
	setObjectDirection(0, DIRECTION_SOUTH)
	scene_started = false
	set_music_volume(1)
	setMilestone(MS_SLEPT_IN_INN, true)
	descriptifyPlayer()
	--print("done all")
end

function start()
	down = Portal:new{x=11, y=3}
	up = Portal:new{x=9, y=3}

	chrisrock = Object:new{x=4, y=5, anim_set="ChrisRock", person=true, move_type=MOVE_WANDER}

	if (not getMilestone(MS_BEACH_BATTLE_DONE)) then
		big_momma = Object:new{x=19, y=9, anim_set="big_momma", person=true}
		farmer = Object:new{x=17, y=9, anim_set="Farmer", person=true}
		setObjectDirection(big_momma.id, DIRECTION_SOUTH)
		setObjectDirection(farmer.id, DIRECTION_SOUTH)
	end

	lady = Object:new{x=2, y=11, anim_set="lady", person=true, dx1=2, dy1=11, dx2=6, dy2=11, dir1=DIRECTION_WEST, dir2=DIRECTION_EAST, move_type=MOVE_LINE, pause=2000, rest=0}
	chest = Chest:new{x=2, y=9, anim_set="chest", milestone=MS_INN1_CHEST, index=ITEM_CURE2}
end

function stop()
end

function update(step)
	down:update()
	up:update()

	if (down.go_time) then
		setMilestone(MS_SLEPT_IN_INN, false)
		change_areas("inn0", 11, 3, DIRECTION_WEST)
		return
	elseif (up.go_time) then
		setMilestone(MS_SLEPT_IN_INN, false)
		change_areas("inn2", 9, 3, DIRECTION_EAST)
		return
	end

	chrisrock:move(step)
	lady:move(step)
	
	if (scene_started) then
		for i, v in ipairs(players) do
			v:update(step)
			if (getMilestone(MS_SLEPT_IN_INN)) then
				return
			end
		end
	end

	px, py = getObjectPosition(0)

	if ((not getMilestone(MS_SLEPT_IN_INN)) and (px == 13) and (py == 5)) then
		if (not scene_started) then
			scriptifyPlayer()

			scene_started = true

			players = {}

			names[1], names[2], names[3], names[4] = getPlayerNames()

			local i

			for i=1,4 do
				if (not (names[i] == nil)) then
					count = count + 1
					if (names[i] == "Eny") then
						players[count] = Object:new{id=0, x=13, y=5, person=true}
					else
						players[count] = Object:new{x=13, y=5, anim_set=names[i], person=true}
					end
					players[count].name = names[i]

					setObjectSolid(players[count].id, false)
					if (count == 1) then
						players[1].scripted_events = {
							{ event_type=EVENT_WALK, dest_x=14, dest_y=5 },
							{ event_type=EVENT_WALK, dest_x=14, dest_y=3 },
							{ event_type=EVENT_WALK, dest_x=13, dest_y=3 },
							{ event_type=EVENT_LOOK, direction=DIRECTION_SOUTH },
							{ event_type=EVENT_REST, delay=1500 },
							{ event_type=EVENT_CUSTOM, callback=do_tint },
							{ event_type=EVENT_GESTURE, name="sleep" },
							{ event_type=EVENT_REST, delay=6000 },
							{ event_type=EVENT_CUSTOM, callback=undo_tint },
							{ event_type=EVENT_WALK, dest_x=14, dest_y=3 },
							{ event_type=EVENT_WALK, dest_x=14, dest_y=5 },
							{ event_type=EVENT_WALK, dest_x=13, dest_y=5 },
							{ event_type=EVENT_LOOK, direction=DIRECTION_SOUTH },
							{ event_type=EVENT_REST, delay=1500 },
							{ event_type=EVENT_CUSTOM, callback=done },
						}
					else
						players[count].scripted_events = {
							{ event_type=EVENT_WALK, dest_x=13+(count+(count-3)), dest_y=5 },
							{ event_type=EVENT_WALK, dest_x=13+(count+(count-3)), dest_y=3 },
							{ event_type=EVENT_WALK, dest_x=13+(count+(count-3))+1, dest_y=3 },
							{ event_type=EVENT_LOOK, direction=DIRECTION_SOUTH },
							{ event_type=EVENT_SYNC, who=players[1], number=8 },
							{ event_type=EVENT_WALK, dest_x=13+(count+(count-3)), dest_y=3 },
							{ event_type=EVENT_WALK, dest_x=13+(count+(count-3)), dest_y=5 },
							{ event_type=EVENT_WALK, dest_x=13, dest_y=5 },
							{ event_type=EVENT_SYNC, who=players[1], number=15 },
						}
					end
				end
			end
		end
	end
end

function activate(activator, activated)
	if (activated == chrisrock.id) then
		setObjectDirection(chrisrock.id, player_dir(chrisrock))
		if (not getMilestone(MS_GOT_KEY) and getMilestone(MS_GOT_RING)) then
			doDialogue("Nice ring! Here, take this key. No, not for free, for the ring!\n", true)
			slot = findUnfullInventorySlot(ITEM_RING)
			setInventory(slot, ITEM_KEY, 1)
			loadPlayDestroy("chest.ogg")
			setMilestone(MS_GOT_KEY, true)
		elseif (not getMilestone(MS_GOT_RING)) then
			doDialogue("Something's missing here... I'm just not sure what.\n")
		else
			doDialogue("Hey, I gave you my key, what more do you want?\n")
		end
	elseif (activated == lady.id) then
		if (getMilestone(MS_BEAT_TODE)) then
			doDialogue("Thank you so much!\n", true)
		else
			doDialogue("I hope the monsters go away soon... there is nothing to do but pace in here!\n", true)
		end
	elseif (activated == chest.id) then
		chest:activate()
	end


	if (not getMilestone(MS_BEACH_BATTLE_DONE)) then
		if (activated == big_momma.id) then
			setObjectDirection(big_momma.id, player_dir(big_momma))
			doDialogue("I could get used to these beds...\nBy the way, you're heading to the beach right? Better have a good strategy... and experience in battle!\n", true)
			setObjectDirection(big_momma.id, DIRECTION_SOUTH)
		elseif (activated == farmer.id) then
			setObjectDirection(farmer.id, player_dir(farmer))
			doDialogue("Farmer: After this all boils over, stop by my farm!\n", true)
			setObjectDirection(farmer.id, DIRECTION_SOUTH)
		end
	end
end

function collide(id1, id2)
end

