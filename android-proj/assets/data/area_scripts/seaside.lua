setMilestone(MS_LEFT_SEASIDE_BY_BOAT, false)
setMilestone(MS_LEFT_EAST_BY_BOAT, false)

music = "burned_village.caf"

talked_to_captain = false
captain_moved = false

local loops = 0
local tiggy_scene_started = false

function end_captain_walk()
	captain_moved = true
	setObjectSolid(captain.id, true)
	setObjectSolid(mate.id, true)
	descriptifyPlayer()
end

function party_appear()
	local px, py = getObjectPosition(0)
	faelon = Object:new{x=px, y=py, anim_set="Faelon", person=true}
	mel = Object:new{x=px, y=py, anim_set="Mel", person=true}
	tig = Object:new{x=px, y=py, anim_set="Tiggy", person=true}
	setObjectSolid(faelon.id, false)
	setObjectSolid(mel.id, false)
	setObjectSolid(tig.id, false)
	faelon.scripted_events = {
		{ event_type=EVENT_WALK, dest_x=px-1, dest_y=py },
		{ event_type=EVENT_LOOK, direction=DIRECTION_NORTH },
		{ event_type=EVENT_SYNC, who=eny, number=5 },
		{ event_type=EVENT_WALK, dest_x=px, dest_y=py },
		{ event_type=EVENT_SYNC, who=eny, number=7 },
	}
	mel.scripted_events = {
		{ event_type=EVENT_WALK, dest_x=px+1, dest_y=py },
		{ event_type=EVENT_LOOK, direction=DIRECTION_NORTH },
		{ event_type=EVENT_SYNC, who=eny, number=5 },
		{ event_type=EVENT_WALK, dest_x=px, dest_y=py },
		{ event_type=EVENT_SYNC, who=eny, number=7 },
	}
	tig.scripted_events = {
		{ event_type=EVENT_WALK, dest_x=px, dest_y=py-1 },
		{ event_type=EVENT_LOOK, direction=DIRECTION_SOUTH },
		{ event_type=EVENT_SYNC, who=eny, number=5 },
		{ event_type=EVENT_WALK, dest_x=px, dest_y=py },
		{ event_type=EVENT_SYNC, who=eny, number=7 },
	}
end

function done_tiggy_scene()
	removeObject(faelon.id)
	removeObject(mel.id)
	removeObject(tig.id)
	descriptifyPlayer()
	setMilestone(MS_TIGGY_SEASIDE_SCENE, true)
	tiggy_scene_started = false
	eny, faelon, mel, tig = nil, nil, nil, nil
	setObjectSolid(0, true)
end

function start()
	if (not getMilestone(MS_SEASIDE_REPAIRED)) then
		addSmoke(10, 6)
		addSmoke(27, 4);
		addSmoke(28, 26)
		informant = Object:new{x=20, y=20, anim_set="Seaside_informant", person=true, move_type=MOVE_WANDER}

		chest0 = Chest:new{x=34, y=23, anim_set="chest", milestone=MS_BURNED_SEASIDE_CHEST_0, index=ITEM_CURE}
		chest1 = Chest:new{x=35, y=23, anim_set="chest", milestone=MS_BURNED_SEASIDE_CHEST_1, index=ITEM_HEAL}
		chest2 = Chest:new{x=36, y=23, anim_set="chest", milestone=MS_BURNED_SEASIDE_CHEST_2, index=ITEM_ELIXIR}

		libPortal = Portal:new{x=28, y=6}
	end

	shopDoor1 = Door:new{x=35, y=8, anim_set="shop_door1"}
	shopDoor2 = Door:new{x=36, y=8, anim_set="shop_door2"}
	shopPortal1 = Portal:new{x=35, y=8}
	shopPortal2 = Portal:new{x=36, y=8}

	captain = Object:new{x=52, y=14, anim_set="Captain", person=true}
	mate = Object:new{x=51, y=14, anim_set="Mate", person=true}
end

function stop()
end

function update(step)
	if (loops > 0) then
		if (not getMilestone(MS_TIGGY_SEASIDE_SCENE) and getMilestone(MS_BEAT_RIDER) and not tiggy_scene_started) then
			stopObject(0)
			scriptifyPlayer()
			local px, py = getObjectPosition(0)
			eny = Object:new{id=0, x=px, y=py}
			eny.scripted_events = {
				{ event_type=EVENT_WALK, dest_x=px, dest_y=py-4 },
				{ event_type=EVENT_CUSTOM, callback=party_appear },
				{ event_type=EVENT_REST, delay=1000 },
				{ event_type=EVENT_SPEAK, text="Tiggy: Oh my... I... uh... I did this?\nEny: No, this staff did this, it only used you as a vessel!\nTiggy: We must destroy this staff... perhaps the wise King would know how?\nFaelon: Indeed, let's head to the East.\n" },
				{ event_type=EVENT_WAIT_FOR_SPEECH },
				{ event_type=EVENT_REST, delay=1000 },
				{ event_type=EVENT_CUSTOM, callback=done_tiggy_scene },
			}
			tiggy_scene_started = true
		end
	else
		loops = loops + 1
	end

	if (tiggy_scene_started) then
		if (not (faelon == nil)) then
			faelon:update(step)
			mel:update(step)
			tig:update(step)
		end
		eny:update(step)
	end

	px, py = getObjectPosition(0)
	if (py >= 34) then
		stopObject(0)
		doMap("seaside")
	end
	
	if (not getMilestone(MS_SEASIDE_REPAIRED)) then
		informant:move(step)
	
		libPortal:update()

		if (libPortal.go_time) then
			change_areas("library_downstairs", 13, 3, DIRECTION_SOUTH);
		end
	end

	if (talked_to_captain and not captain_moved) then
		captain:update(step)
		mate:update(step)
	end

	shopPortal1:update()
	shopPortal2:update()

	if	( 
		(shopDoor1 == nil and shopPortal1.go_time) or
		(shopDoor2 == nil and shopPortal2.go_time)
		)
	then
		change_areas("seaside_shop", 4, 9, DIRECTION_NORTH);
	end

	px, py = getObjectPosition(0)
	if ((px == 51 and py == 14) or (px == 52 and py == 14)) then
		setGlobalCanSave(false)
	elseif ((px == 51 and py == 15) or (px == 52 and py == 15)) then
		setGlobalCanSave(true)
	end
end

function activate(activator, activated)
	if (not getMilestone(MS_SEASIDE_REPAIRED)) then
		if (activated == informant.id) then
			doDialogue("When the fires started everyone went south.\n", true)
			if (not getMilestone(MS_GOT_BADGE) and getMilestone(MS_UNDERGROUND_CHEST_2)) then
				doDialogue("Say, is that a letter?\nThat looks like my father's writing!\n... it's written to me!\nSay, if you're going east, you may need this badge...\nFair trade? O.K.\n", true)
				slot = findUnfullInventorySlot(ITEM_LETTER)
				setInventory(slot, ITEM_BADGE, 1)
				loadPlayDestroy("chest.ogg")
				setMilestone(MS_GOT_BADGE, true)
			end
		elseif (activated == chest0.id) then
			chest0:activate()
		elseif (activated == chest1.id) then
			chest1:activate()
		elseif (activated == chest2.id) then
			chest2:activate()
		end
	end

	if (activated == captain.id) then
		if (getMilestone(MS_RIOS_JOINED)) then
			if (not talked_to_captain) then
				scriptifyPlayer()
				doDialogue("Captain: You want to board the ship? Aye, we set sail as soon as you're ready!\n", true, false, true)
				setObjectSolid(captain.id, false)
				setObjectSolid(mate.id, false)
				captain.scripted_events = {
					{event_type=EVENT_WALK, dest_x=52, dest_y=12},
					{event_type=EVENT_WALK, dest_x=58, dest_y=12},
					{event_type=EVENT_WALK, dest_x=58, dest_y=9},
					{event_type=EVENT_LOOK, direction=DIRECTION_EAST},
				}
				mate.scripted_events = {
					{event_type=EVENT_SYNC, who=captain, number=1},
					{event_type=EVENT_WALK, dest_x=51, dest_y=12},
					{event_type=EVENT_WALK, dest_x=58, dest_y=12},
					{event_type=EVENT_WALK, dest_x=58, dest_y=10},
					{event_type=EVENT_LOOK, direction=DIRECTION_EAST},
					{event_type=EVENT_CUSTOM, callback=end_captain_walk},
				}
				talked_to_captain = true
			elseif (captain_moved) then
				setMilestone(MS_TALKED_TO_CAPTAIN, true)
				setMilestone(MS_LEFT_SEASIDE_BY_BOAT, true)
				doDialogue("Captain: Alright then, off we go!\n", true)
				setGlobalCanSave(true)
				doMap("seaside")
			end
		else
			doDialogue("Captain: We can't sail yet!\nMate: We don't have a Cleric aboard.\n")
		end
	elseif (activated == mate.id) then
		if (not getMilestone(MS_MATE_GAVE_ADVICE)) then
			setMilestone(MS_MATE_GAVE_ADVICE, true)
			doDialogue("Mate: Sharks, crabs... these things are much more than just trophies!\n")
		else
			doDialogue("Mate: G'day! ...\n")
		end
	end
end

function collide(id1, id2)
	if ((not (shopDoor1 == nil)) and (id1 == shopDoor1.id or id2 == shopDoor1.id)) then
		shopDoor1:open()
		removeObject(shopDoor1.id)
		shopDoor1 = nil
	elseif ((not (shopDoor2 == nil)) and (id1 == shopDoor2.id or id2 == shopDoor2.id)) then
		shopDoor2:open()
		removeObject(shopDoor2.id)
		shopDoor2 = nil
	end
end

