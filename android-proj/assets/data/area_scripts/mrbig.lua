music = "Muttrace.ogg"

local count = 0
local scene_started = false

function start()
	out = Portal:new{x=7, y=19}
	albert = Object:new{x=7, y=4, anim_set="Albert", person=true, direction=DIRECTION_SOUTH}
	dudes = {}
	dudes[1] = Object:new{x=3, y=8, anim_set="Doggy", person=true, direction=DIRECTION_SOUTH}
	dudes[2] = Object:new{x=3, y=11, anim_set="Kitty", person=true, direction=DIRECTION_SOUTH}
	dudes[3] = Object:new{x=3, y=14, anim_set="Doggy", person=true, direction=DIRECTION_SOUTH}
	dudes[4] = Object:new{x=11, y=8, anim_set="Kitty", person=true, direction=DIRECTION_SOUTH}
	dudes[5] = Object:new{x=11, y=11, anim_set="Doggy", person=true, direction=DIRECTION_SOUTH}
	dudes[6] = Object:new{x=11, y=14, anim_set="Kitty", person=true, direction=DIRECTION_SOUTH}

	chest = Chest:new{x=1, y=2, anim_set="chest", milestone=MS_MRBIG_CHEST, index=ITEM_STAFF}
end

function stop()
end

function look1()
	setObjectDirection(dudes[3].id, DIRECTION_EAST)
	setObjectDirection(dudes[6].id, DIRECTION_WEST)
end

function look2()
	setObjectDirection(dudes[3].id, DIRECTION_NORTH)
	setObjectDirection(dudes[6].id, DIRECTION_NORTH)
	setObjectDirection(dudes[2].id, DIRECTION_EAST)
	setObjectDirection(dudes[5].id, DIRECTION_WEST)
end

function look3()
	setObjectDirection(dudes[2].id, DIRECTION_NORTH)
	setObjectDirection(dudes[5].id, DIRECTION_NORTH)
	setObjectDirection(dudes[1].id, DIRECTION_EAST)
	setObjectDirection(dudes[4].id, DIRECTION_WEST)
end

function look_final()
	local i
	for i=1,6 do
		setObjectDirection(dudes[i].id, DIRECTION_SOUTH)
	end
end

function done()
	removeObject(tiggy.id)
	removeObject(tipper.id)
	removeObject(mel.id)
	tiggy = nil
	tipper = nil
	mel = nil
	look_final()
	descriptifyPlayer()
	setMilestone(MS_MUTTRACE_SCENE, true)
	scene_started = false
end

function look4()
	setObjectDirection(dudes[1].id, DIRECTION_NORTH)
	setObjectDirection(dudes[4].id, DIRECTION_NORTH)

	-- bring out all players
	local px, py = getObjectPosition(0)
	tiggy = Object:new{x=px, y=py, anim_set="Tiggy", person=true, direction=DIRECTION_NORTH}
	tipper = Object:new{x=px, y=py, anim_set="Tipper", person=true, direction=DIRECTION_NORTH}
	mel = Object:new{x=px, y=py, anim_set="Mel", person=true, direction=DIRECTION_NORTH}

	setObjectSolid(tiggy.id, false)
	setObjectSolid(tipper.id, false)
	setObjectSolid(mel.id, false)

	tiggy.scripted_events = {
		{ event_type=EVENT_WALK, dest_x=px-1, dest_y=py },
		{ event_type=EVENT_LOOK, direction=DIRECTION_NORTH },
		{ event_type=EVENT_SYNC, who=eny, number=11 },
		{ event_type=EVENT_WALK, dest_x=px, dest_y=py },
	}

	tipper.scripted_events = {
		{ event_type=EVENT_WALK, dest_x=px+1, dest_y=py },
		{ event_type=EVENT_LOOK, direction=DIRECTION_NORTH },
		{ event_type=EVENT_SYNC, who=eny, number=11 },
		{ event_type=EVENT_WALK, dest_x=px, dest_y=py },
	}

	mel.scripted_events = {
		{ event_type=EVENT_WALK, dest_x=px+2, dest_y=py },
		{ event_type=EVENT_LOOK, direction=DIRECTION_NORTH },
		{ event_type=EVENT_SYNC, who=eny, number=11 },
		{ event_type=EVENT_WALK, dest_x=px, dest_y=py },
	}

	eny.scripted_events[#eny.scripted_events+1] =
		{ event_type=EVENT_SYNC, who=mel, number=1 };
	eny.scripted_events[#eny.scripted_events+1] =
		{ event_type=EVENT_SPEAK, text="Albert: Hello friends.\nEny: Hello. We have come in search of a staff that landed here a few days ago.\nAlbert: I have the Staff you speak of.\nEny: We must have it. It must be destroyed. It has evil powers.\nAlbert: Indeed. But the Staff's power is only present on your planet.\nEny: We need to destroy the staff. Do you know how we could do that?\nAlbert: You would have to fly it into the nearest Star, but we have no ships in this day and age.\nEny: We have a craft, but it is our only transport home...\nAlbert: Well, there is one other way...\nEny: What is it?\nAlbert: There is a Portal to your planet on the Dark Side... But it is very dangerous!\nEny: We no longer fear danger. We will brave the Dark Side after we send the staff towards the Sun.\nAlbert: Very well. You may have the Staff. It's in that chest by the window.\n" };
	eny.scripted_events[#eny.scripted_events+1] =
		{ event_type=EVENT_WAIT_FOR_SPEECH };
	eny.scripted_events[#eny.scripted_events+1] =
		{ event_type=EVENT_SYNC, who=mel, number=4 };
	eny.scripted_events[#eny.scripted_events+1] =
		{ event_type=EVENT_CUSTOM, callback=done };
end

function update(step)
	if (count == 1 and not getMilestone(MS_MUTTRACE_SCENE)) then
		local px, py = getObjectPosition(0)
		eny = Object:new{id=0, x=px, y=py, person=true}
		stopObject(0)
		scriptifyPlayer()
		eny.scripted_events = {
			{ event_type=EVENT_WALK, dest_x=7, dest_y=14 },
			{ event_type=EVENT_CUSTOM, callback=look1 },
			{ event_type=EVENT_WALK, dest_x=7, dest_y=11 },
			{ event_type=EVENT_CUSTOM, callback=look2 },
			{ event_type=EVENT_WALK, dest_x=7, dest_y=8 },
			{ event_type=EVENT_CUSTOM, callback=look3 },
			{ event_type=EVENT_WALK, dest_x=7, dest_y=6 },
			{ event_type=EVENT_CUSTOM, callback=look4 },
		}
		scene_started = true
	end

	count = count + 1

	if (scene_started) then
		eny:update(step)
		if (not (tiggy == nil)) then
			tiggy:update(step)
			tipper:update(step)
			mel:update(step)
		end
	end

	if (out:update()) then
		change_areas("Muttrace", 20, 18, DIRECTION_SOUTH)
	end
end

function activate(activator, activated)
	if (activated == albert.id) then
		local _d = getObjectDirection(albert.id)
		setObjectDirection(albert.id, player_dir(albert))
		if (getMilestone(MS_BEAT_TODE)) then
			doDialogue("Albert: You are truly heroes!\n", true)
		else
			doDialogue("Albert: We all wish you the best!\n", true)
		end
		setObjectDirection(albert.id, _d)
	elseif (activated == chest.id) then
		chest:activate()
	else
		local i
		for i=1,6 do
			if (activated == dudes[i].id) then
				setObjectDirection(dudes[i].id, player_dir(dudes[i]))
				if (i == 1) then
					doDialogue( "King Albert is very wise.\n", true)
				elseif (i == 2) then
					doDialogue( "King Albert is a descendant of the maker of the staff.\n", true)
				elseif (i == 3) then
					doDialogue( "We all wish to be as kind and knowledgeable as King Albert!\n", true)
				elseif (i == 4) then
					doDialogue( "Thank you for visiting our King...\n", true)
				elseif (i == 5) then
					doDialogue( "I was so happy when the Staff returned!\n", true)
				elseif (i == 6) then
					doDialogue( "King Albert's ancestor traded the Staff for building materials long ago...\n", true)
				end
				setObjectDirection(dudes[i].id, DIRECTION_SOUTH)
			end
		end
	end
end

function collide(id1, id2)
end

