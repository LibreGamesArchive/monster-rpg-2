talked_to_captain = false
music = "dockknocking.ogg"

started = false
first_added = false
second_added = false
fleer_gone = false
faelon_gone = false
rider_gone = false


function end_captain_walk()
	captain_moved = true
	descriptifyPlayer()
end

function fan_out()
	-- hack
	setObjectDirection(0, DIRECTION_SOUTH)

	first_added = true

	local px, py = getObjectPosition(0)

	local rider2 = Object:new{x=px, y=py, anim_set="Rider", person=true}
	local faelon2 = Object:new{x=px, y=py, anim_set="Faelon", person=true}
	setObjectSolid(rider2.id, false)
	setObjectSolid(faelon2.id, false)

	for k, v in pairs(rider2) do
		rider[k] = v
	end
	for k, v in pairs(faelon2) do
		faelon[k] = v
	end

	rider.scripted_events = {
		{ event_type=EVENT_WALK, dest_x=px-1, dest_y=py },
		{ event_type=EVENT_LOOK, direction=DIRECTION_SOUTH },
		{ event_type=EVENT_SYNC, who=eny, number=8 },
		{ event_type=EVENT_SYNC, who=faelon, number=6 },
		{ event_type=EVENT_WALK, dest_x=24, dest_y=16 },
		{ event_type=EVENT_WALK, dest_x=24, dest_y=17 },
		{ event_type=EVENT_CUSTOM, callback=remove_rider },
		{ event_type=EVENT_SYNC, who=eny, number=13 },
	}

	faelon.scripted_events = {
		{ event_type=EVENT_WALK, dest_x=px, dest_y=py-1 },
		{ event_type=EVENT_LOOK, direction=DIRECTION_SOUTH },
		{ event_type=EVENT_SYNC, who=eny, number=8 },
		{ event_type=EVENT_SYNC, who=fleer, number=6 },
		{ event_type=EVENT_WALK, dest_x=24, dest_y=17 },
		{ event_type=EVENT_CUSTOM, callback=remove_faelon },
		{ event_type=EVENT_SYNC, who=eny, number=13 },
	}
	
	setObjectSubAnimation(sub.id, "with_gunnar")
end

function add_mel()
	second_added = true

	setObjectPosition(fleer.id, 48, 16)

	px, py = getObjectPosition(0)
	fleer.scripted_events = {
		{ event_type=EVENT_WALK, dest_x=px+1, dest_y=py },
		{ event_type=EVENT_LOOK, direction=DIRECTION_WEST },
		{ event_type=EVENT_SYNC, who=eny, number=11 },
		{ event_type=EVENT_WALK, dest_x=24, dest_y=16 },
		{ event_type=EVENT_WALK, dest_x=24, dest_y=17 },
		{ event_type=EVENT_CUSTOM, callback=remove_fleer },
		{ event_type=EVENT_SYNC, who=eny, number=13 },
	}
end


function bye_gunnar()
	setObjectSubAnimation(sub.id, "bob")
end

function remove_eny()
	setObjectHidden(0, true)
end

function remove_fleer()
	removeObject(fleer.id)
	fleer_gone = true
	addPartyMember("Mel");
end

function remove_faelon()
	removeObject(faelon.id)
	faelon_gone = true
end

function remove_rider()
	removeObject(rider.id)
	rider_gone = true
end

function end_sub_scene()
	--
	local result = prompt("Skip", "this mini game?", 0, 0)
	if (result == true) then
		bonusPoints()
		setObjectHidden(0, false)
		setObjectSolid(0, true)
		change_areas("fort_start", 36, 47, DIRECTION_SOUTH)
	else
	--
		dpad_off()
		drawArea()
		dpad_on()
		fadeOut(0, 0, 0)
		setObjectHidden(0, false)
		setObjectSolid(0, true)
		setMilestone(MS_DOCK_TO_FORT, true)
		if (doShooter(true)) then
			change_areas("fort_start", 36, 47, DIRECTION_SOUTH)
		else
			gameOver();
		end
	end
end


function start()
	-- work around a bug where you'd get stranded on top of
	-- submarine and couldn't get back to land
	local bug_x, bug_y = getObjectPosition(0)
	if (bug_x == 24 and bug_y == 17) then
		setObjectPosition(0, 24, 15)
	end

	captain = Object:new{x=18, y=14, anim_set="Captain", person=true}
	mate = Object:new{x=17, y=14, anim_set="Mate", person=true}

	setMilestone(MS_LEFT_EAST_BY_BOAT, false)
	setMilestone(MS_LEFT_SEASIDE_BY_BOAT, false)
	setMilestone(MS_FORT_TO_DOCK, false)
	setMilestone(MS_DOCK_TO_FORT, false)

	if (getMilestone(MS_SUB_SCENE_DONE)) then
		sub = Object:new{x=22, y=19, anim_set="sub"}
		setObjectLow(sub.id, true)
		gunnar = Object:new{x=24, y=16, anim_set="Gunnar", person=true}
		setObjectDirection(gunnar.id, DIRECTION_NORTH)
	elseif (getMilestone(MS_GUNNAR_LEFT) and not getMilestone(MS_SUB_SCENE_DONE)) then
		sub = Object:new{x=22, y=19, anim_set="sub"}
		setObjectLow(sub.id, true)
		fleer = Object:new{x=42, y=14, anim_set="fleer", person=true}
		setObjectDirection(fleer.id, DIRECTION_SOUTH)
	elseif (not getMilestone(MS_SUB_SCENE_DONE)) then
		fleer = Object:new{x=42, y=16, anim_set="fleer", person=true, move_type=MOVE_WANDER}
	end
end

function stop()
end

function update(step)
	if (not getMilestone(MS_GUNNAR_LEFT)) then
		fleer:move(step)
	end

	if (not getMilestone(MS_SAILORS_WORD)) then
		doDialogue("Captain: We'll be here for you mate, if you need our services again.\n")
		setMilestone(MS_SAILORS_WORD, true)
	end

	px, py = getObjectPosition(0)
	if (px >= 50) then
		stopObject(0)
		doMap("eastern_dock")
	end
	
	if (talked_to_captain and not captain_moved) then
		captain:update(step)
		mate:update(step)
	end

	if (getMilestone(MS_GUNNAR_LEFT) and not getMilestone(MS_SUB_SCENE_DONE)) then
		if (not started) then
			px, py = getObjectPosition(0)
			if (px < 32) then
				started = true
				scriptifyPlayer()
				eny = Object:new{id=0, x=px, y=py}
				setObjectSolid(eny.id, false)
				setObjectSolid(fleer.id, false)

				rider = Object:new{}
				faelon = Object:new{}

				eny.scripted_events = {
					{ event_type=EVENT_WALK, dest_x=24, dest_y=16 },
					{ event_type=EVENT_CUSTOM, callback=fan_out },
					{ event_type=EVENT_SPEAK, text="Gunnar: So, what do you think?\nRider: Is that thing seaworthy?!\nGunnar: Absolutely...\nEny: Well if it can get us to the floating fortress it will have done it's job.\nFaelon: Gunnar, are you equipped for battle?\nGunnar: No, I will have to stay with the sub, or there's no getting off of that fortress...\nFaelon: Hmm, then we need some one else...\n" },
					{ event_type=EVENT_WAIT_FOR_SPEECH },
					{ event_type=EVENT_CUSTOM, callback=add_mel },
					{ event_type=EVENT_SYNC, who=fleer, number=2 },
					{ event_type=EVENT_SPEAK, text="Mel: I hear you're looking for help...\nRider: What help could you be?\nMel: I happen to be very skilled with magic!\nEny: You're a young girl, shouldn't you be home with your parents?\nMel: I don't have parents... I have no family at all...\nEny: Oh my... What happened?\nMel: Monsters got them...\nEny: So you think you have a score to settle then?\nMel: Yes. \\ Eny: Alright then, you can come.\nEny: Let's go straight away, we have no time to lose!\nGunnar: All aboard!\n" },
					{ event_type=EVENT_WAIT_FOR_SPEECH },
					{ event_type=EVENT_CUSTOM, callback=bye_gunnar },
					{ event_type=EVENT_WALK, dest_x=24, dest_y=17 },
					{ event_type=EVENT_CUSTOM, callback=remove_eny },
					{ event_type=EVENT_SYNC, who=rider, number=7 },
					{ event_type=EVENT_CUSTOM, callback=end_sub_scene },
				}
			end
		end

		if (started) then

			eny:update(step)
			if (not getMilestone(MS_SUB_SCENE_DONE)) then
				if (first_added) then
					if (not rider_gone) then
						rider:update(step)
					end
					if (not faelon_gone) then
						faelon:update(step)
					end
				end
				if (second_added) then
					if (not fleer_gone) then
						fleer:update(step)
					end
				end
			end
		end
	end

	px, py = getObjectPosition(0)
	if ((px == 17 and py == 14) or (px == 18 and py == 14)) then
		setGlobalCanSave(false)
	elseif ((px == 17 and py == 15) or (px == 18 and py == 15)) then
		setGlobalCanSave(true)
	end
end

function activate(activator, activated)
	if (activated == captain.id) then
		if (not talked_to_captain) then
			scriptifyPlayer()
			doDialogue("Captain: You want to board the ship? Aye, we set sail as soon as you're ready!\n", true, false, true)
			captain.scripted_events = {
				{event_type=EVENT_WALK, dest_x=18, dest_y=12},
				{event_type=EVENT_WALK, dest_x=24, dest_y=12},
				{event_type=EVENT_WALK, dest_x=24, dest_y=9},
				{event_type=EVENT_LOOK, direction=DIRECTION_WEST},
			}
			mate.scripted_events = {
				{event_type=EVENT_SYNC, who=captain, number=1},
				{event_type=EVENT_WALK, dest_x=17, dest_y=12},
				{event_type=EVENT_WALK, dest_x=24, dest_y=12},
				{event_type=EVENT_WALK, dest_x=24, dest_y=10},
				{event_type=EVENT_LOOK, direction=DIRECTION_SOUTH},
				{event_type=EVENT_CUSTOM, callback=end_captain_walk},
			}
			talked_to_captain = true
		elseif (captain_moved) then
			setMilestone(MS_LEFT_EAST_BY_BOAT, true)
			doDialogue("Captain: Alright then, off we go!\n", true)
			setGlobalCanSave(true)
			doMap("eastern_dock")
		end
	elseif (activated == mate.id) then
		doDialogue("Mate: G'day! ...\n")
	elseif (fleer and activated == fleer.id) then
		setObjectDirection(fleer.id, player_dir(fleer))
		smartDialogue{"Girl: There are monsters attacking people in the area near Flowey.\nGirl: The King will not believe that monsters have risen again!\nGirl: The people of Flowey cannot hold the city much longer!\n",
			MS_GUNNAR_LEFT, "Girl: Sure Flowey is safer now... but how long will it last?\n"
		}
	elseif (gunnar and activated == gunnar.id) then
		local _d = getObjectDirection(gunnar.id)
		setObjectDirection(gunnar.id, player_dir(gunnar))
		if (getMilestone(MS_BEAT_TODE)) then
			doDialogue("Gunnar: Wow! What a feat of engineering!\n", true)
		elseif (getMilestone(MS_BEAT_GIRL_DRAGON)) then
			doDialogue("Gunnar: No, I've never built a space craft... but I know someone who has.\nGunnar: Odd thing is he's a simple farmer!\n", true);
		end
		local choice = triple_prompt(
			"Would you like to go through",
			"the trench again, or take the",
			"longer, safer route?",
			"Trench", "Safe", "Cancel", 2)
		setObjectDirection(gunnar.id, _d)
		if (choice == 0) then
			if (doShooter(false)) then
				change_areas("fort_start", 36, 47, DIRECTION_SOUTH)
			else
				gameOver();
			end
		elseif (choice == 1) then
			setMilestone(MS_FORT_TO_DOCK, false)
			setMilestone(MS_DOCK_TO_FORT, true)
			doMap("eastern_dock")
		end
	end
end

function collide(id1, id2)
end

