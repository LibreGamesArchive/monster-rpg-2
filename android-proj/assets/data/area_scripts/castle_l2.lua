music = "castle.caf"

party_added = false

second_scene_start = false
gunnar_added = false
gunnar_removed = false

local third_scene_started = false


function remove_gunnar()
	removeObject(gunnar.id)
	gunnar_removed = true
	removePlayer("Gunnar")
end


function add_gunnar()
	gunnar_added = true
	px, py = getObjectPosition(0)
	gunnar = Object:new{x=px, y=py, anim_set="Gunnar", person=true}
	setObjectSolid(gunnar.id, false)

	gunnar.scripted_events = {
		{ event_type=EVENT_WALK, dest_x=px+1, dest_y=py },
		{ event_type=EVENT_LOOK, direction=DIRECTION_WEST },
		{ event_type=EVENT_SPEAK, text="Gunnar: I may have a solution...\nEny: Oh? What?\nGunnar: I have invented an underwater vehicle that is fast and packs more firepower than the largest traditional vessel.\nEny: Well, it sounds like the only hope we have...\nGunnar: Absolutely... I will prepare the craft then, and meet you at the docks when you are ready!\nEny: Thank you Gunnar, you have gone far beyond the call of duty.\n" },
		{ event_type=EVENT_WAIT_FOR_SPEECH },
		{ event_type=EVENT_WALK, dest_x=px+1, dest_y=py+12 },
		{ event_type=EVENT_CUSTOM, callback=remove_gunnar },
	}

	eny.scripted_events[6].who = gunnar -- hack
end


function add_party()
	rider = Object:new{x=8, y=12, anim_set="Rider", person=true}
	setObjectSolid(rider.id, false)
	rider.scripted_events = {
		{ event_type=EVENT_WALK, dest_x=7, dest_y=11 },
		{ event_type=EVENT_LOOK, direction=DIRECTION_NORTH },
		{ event_type=EVENT_SYNC, who=eny, number=4 },
		{ event_type=EVENT_WALK, dest_x=7, dest_y=7 },
		{ event_type=EVENT_SYNC, who=eny, number=10 },
		{ event_type=EVENT_WALK, dest_x=8, dest_y=8 },
	}

	rios = Object:new{x=8, y=12, anim_set="Rios", person=true}
	setObjectSolid(rios.id, false)
	rios.scripted_events = {
		{ event_type=EVENT_WALK, dest_x=10, dest_y=12 },
		{ event_type=EVENT_LOOK, direction=DIRECTION_NORTH },
		{ event_type=EVENT_SYNC, who=eny, number=4 },
		{ event_type=EVENT_WALK, dest_x=10, dest_y=8 },
		{ event_type=EVENT_SYNC, who=eny, number=10 },
		{ event_type=EVENT_WALK, dest_x=8, dest_y=8 },
	}

	if (getMilestone(MS_GUNNAR_JOINED)) then
		gunnar = Object:new{x=8, y=12, anim_set="Gunnar", person=true}
		setObjectSolid(gunnar.id, false)
		gunnar.scripted_events = {
			{ event_type=EVENT_WALK, dest_x=11, dest_y=11 },
			{ event_type=EVENT_LOOK, direction=DIRECTION_NORTH },
			{ event_type=EVENT_SYNC, who=eny, number=4 },
			{ event_type=EVENT_WALK, dest_x=11, dest_y=7 },
			{ event_type=EVENT_SYNC, who=eny, number=10 },
			{ event_type=EVENT_WALK, dest_x=8, dest_y=8 },
		}
	end

	party_added = true
end

function end_scene()
	removeObject(faelon.id)
	removeObject(rider.id)
	removeObject(rios.id)
	if (getMilestone(MS_GUNNAR_JOINED)) then
		removeObject(gunnar.id)
	end
	descriptifyPlayer()
	setMilestone(MS_ENDED_THRONE_SCENE, true)
end

function end_scene2()
	removeObject(faelon.id)
	descriptifyPlayer()
	setMilestone(MS_GUNNAR_LEFT, true)
	addPartyMember("Faelon")
end

function add_party2()
	local px, py = getObjectPosition(0)

	faelon = Object:new{x=px, y=py, anim_set="Faelon", person=true}
	tig = Object:new{x=px, y=py, anim_set="Tiggy", person=true}
	mel = Object:new{x=px, y=py, anim_set="Mel", person=true}
	
	setObjectSolid(faelon.id, false)
	setObjectSolid(tig.id, false)
	setObjectSolid(mel.id, false)

	faelon.scripted_events = {
		{ event_type=EVENT_WALK, dest_x=px-1, dest_y=py },
		{ event_type=EVENT_LOOK, direction=DIRECTION_NORTH },
		{ event_type=EVENT_SYNC, who=eny, number=4 },
		{ event_type=EVENT_WALK, dest_x=7, dest_y=7 },
		{ event_type=EVENT_SYNC, who=eny, number=7 },
		{ event_type=EVENT_WALK, dest_x=8, dest_y=7 },
		{ event_type=EVENT_SYNC, who=eny, number=9 },
	}

	tig.scripted_events = {
		{ event_type=EVENT_WALK, dest_x=px+1, dest_y=py },
		{ event_type=EVENT_LOOK, direction=DIRECTION_NORTH },
		{ event_type=EVENT_SYNC, who=eny, number=4 },
		{ event_type=EVENT_WALK, dest_x=9, dest_y=7 },
		{ event_type=EVENT_SYNC, who=eny, number=7 },
		{ event_type=EVENT_WALK, dest_x=8, dest_y=7 },
		{ event_type=EVENT_SYNC, who=eny, number=9 },
	}

	mel.scripted_events = {
		{ event_type=EVENT_WALK, dest_x=px+2, dest_y=py },
		{ event_type=EVENT_LOOK, direction=DIRECTION_NORTH },
		{ event_type=EVENT_SYNC, who=eny, number=4 },
		{ event_type=EVENT_WALK, dest_x=10, dest_y=7 },
		{ event_type=EVENT_SYNC, who=eny, number=7 },
		{ event_type=EVENT_WALK, dest_x=8, dest_y=7 },
		{ event_type=EVENT_SYNC, who=eny, number=9 },
	}
end


function done_third_scene()
	descriptifyPlayer()
	setObjectSolid(0, true)
	removeObject(tig.id)
	removeObject(faelon.id)
	removeObject(mel.id)
	eny, tig, faelon, mel = nil, nil, nil, nil
	setMilestone(MS_SPOKE_TO_KING_ABOUT_STAFF, true)
	third_scene_started = false
end


function start()
	King = Object:new{x=7, y=5, anim_set="King", person=true}
	downLeftPortal = Portal:new{x=3, y=18, width=1, height=1}
	downRightPortal = Portal:new{x=15, y=18, width=1, height=1}
	outLeftPortal = Portal:new{x=1, y=18, width=1, height=1}
	outRightPortal = Portal:new{x=17, y=18, width=1, height=1}

	if ((getMilestone(MS_BEACH_BATTLE_DONE) and not getMilestone(MS_GUNNAR_LEFT)) or getMilestone(MS_BEAT_TODE)) then
		faelon = Object:new{x=9, y=5, anim_set="Faelon", person=true, direction=DIRECTION_SOUTH}
	end

	door = Object:new{x=9, y=2, width=1, height=1}
end

function stop()
end

function update(step)
	downLeftPortal:update()
	downRightPortal:update()
	outLeftPortal:update()
	outRightPortal:update()

	if (downLeftPortal.go_time) then
		change_areas("castle_l1", 11, 31, DIRECTION_NORTH)
	end

	if (downRightPortal.go_time) then
		change_areas("castle_l1", 23, 31, DIRECTION_NORTH)
	end

	if (outLeftPortal.go_time) then
		change_areas("castle", 18, 27, DIRECTION_SOUTH)
	end

	if (outRightPortal.go_time) then
		change_areas("castle", 27, 27, DIRECTION_SOUTH)
	end

	if ((not getMilestone(MS_STARTED_THRONE_SCENE)) and (not getMilestone(MS_BEACH_BATTLE_DONE))) then
		px, py = getObjectPosition(0)
		if (py <= 12) then
			setMilestone(MS_STARTED_THRONE_SCENE, true)
	
			stopObject(0)
			scriptifyPlayer()
			eny = Object:new{id=0, x=px, y=py}
			faelon = Object:new{x=9, y=18, anim_set="Faelon", person=true}
			setObjectDirection(eny.id, DIRECTION_NORTH)

			eny.scripted_events = {
				{ event_type=EVENT_WALK, dest_x=8, dest_y=12 },
				{ event_type=EVENT_LOOK, direction=DIRECTION_NORTH },
				{ event_type=EVENT_CUSTOM, callback=add_party },
				{ event_type=EVENT_REST, delay=2000 },
				{ event_type=EVENT_WALK, dest_x=8, dest_y=8 },
				{ event_type=EVENT_REST, delay=200 },
				{ event_type=EVENT_SPEAK, text="Eny: We humbly request your help Sire. Flowey is under attack.\nEny: The people are holding the city but they can't hold it alone!\nKing: Hmm... I have heard the rumors, but how is one to believe?\nKing: If you could prove to me that there are monsters walking the earth once again, I would gladly send troops...\n" },
				{ event_type=EVENT_SYNC, who=faelon, number=5 },
				{ event_type=EVENT_SPEAK, text="King: Please, take your rest in the castle... stay as long as you like...\n" },
				{ event_type=EVENT_WAIT_FOR_SPEECH },
				{ event_type=EVENT_REST, delay=2000 },
				{ event_type=EVENT_CUSTOM, callback=end_scene },
			}



			faelon.scripted_events = {
				{ event_type=EVENT_SYNC, who=eny, number=7 },
				{ event_type=EVENT_WALK, dest_x=9, dest_y=6 },
				{ event_type=EVENT_LOOK, direction=DIRECTION_SOUTH },
				{ event_type=EVENT_SPEAK, text="Faelon: My Father, surely you don't believe these claims? Monsters walking the earth? It has been many years since!\nKing: Well... umm... I suppose you are right, son. I have left the troops under your control, so I trust your judgement.\nFaelon: Thank you Father. Please, my Father must rest, be on your way...\n" },
				{ event_type=EVENT_WALK, dest_x=9, dest_y=18 },

			}
		end
	else
		if( (not getMilestone(MS_ENDED_THRONE_SCENE)) and (not getMilestone(MS_BEACH_BATTLE_DONE))) then
			eny:update(step)
			if (not getMilestone(MS_ENDED_THRONE_SCENE)) then
				if (party_added) then
					rider:update(step)
					rios:update(step)
					if (getMilestone(MS_GUNNAR_JOINED)) then
						gunnar:update(step)
					end
				end
				faelon:update(step)
			end
		elseif (getMilestone(MS_BEACH_BATTLE_DONE) and not getMilestone(MS_GUNNAR_LEFT)) then
			if (second_scene_started) then
				eny:update(step)
				faelon:update(step)
				if (gunnar_added and not gunnar_removed) then
					gunnar:update(step)
				end
			else
				local px, py = getObjectPosition(0)
				if (py <= 12) then
					second_scene_started = true

					stopObject(0)
					scriptifyPlayer()
					eny = Object:new{id=0, x=px, y=py}
					setObjectDirection(eny.id, DIRECTION_NORTH)

					eny.scripted_events = {
						{ event_type=EVENT_WALK, dest_x=7, dest_y=8 },
						{ event_type=EVENT_LOOK, direction=DIRECTION_NORTH },
						{ event_type=EVENT_SPEAK, text="Eny: We have returned from Flowey with confirmation of the enemy invasion...\nKing: Save your breath child, we now know of the crisis to the north.\nKing: I deeply regret not sending troops to battle the monsters... but I hear you have quelled the immediate danger.\nEny: Yes, but we fear it will not stop until we uproot the evil that is in the staff my good friend holds.\nKing: Well, you may be in luck. A fisherman was here not an hour ago telling of a floating fortress in the waters up north.\nEny: But the seas are swarming with threats, we should be so lucky as him to escape them safely!...\n" },
						{ event_type=EVENT_WAIT_FOR_SPEECH },
						{ event_type=EVENT_CUSTOM, callback=add_gunnar },
						{ event_type=EVENT_SYNC, who=nil, number=5 },
						{ event_type=EVENT_SPEAK, text="Eny: Well, for now we are short-handed...\nFaelon: I would be honored to serve my King by helping you!\nEny: That's great!\nFaelon: We should prepare supplies and weapons, and then leave immediately.\nFaelon: The shop may have something new you can use.\nEny: Yes, let us go...\n" },
						{ event_type=EVENT_SYNC, who=faelon, number=3 },
						{ event_type=EVENT_CUSTOM, callback=end_scene2 },
					}

					setObjectSolid(faelon.id, false)

					faelon.scripted_events = {
						{ event_type=EVENT_SYNC, who=eny, number=7 },
						{ event_type=EVENT_WALK, dest_x=9, dest_y=8 },
						{ event_type=EVENT_WALK, dest_x=7, dest_y=8 },
						{ event_type=EVENT_SYNC, who=eny, number=9 },
					}
				end
			end
		end
	end

	local px, py = getObjectPosition(0)
	if (not getMilestone(MS_SPOKE_TO_KING_ABOUT_STAFF) and getMilestone(MS_BEAT_RIDER) and not third_scene_started and not getMilestone(MS_BEAT_TODE)
			and py <= 12) then
		third_scene_started = true

		stopObject(0)
		scriptifyPlayer()
		eny = Object:new{id=0, x=px, y=py}
		setObjectDirection(eny.id, DIRECTION_NORTH)

		setObjectSolid(eny.id, false)
		
		eny.scripted_events = {
			{ event_type=EVENT_WALK, dest_x=8, dest_y=12 },
			{ event_type=EVENT_LOOK, direction=DIRECTION_NORTH },
			{ event_type=EVENT_CUSTOM, callback=add_party2 },
			{ event_type=EVENT_REST, delay=1000 },
			{ event_type=EVENT_WALK, dest_x=8, dest_y=7 },
			{ event_type=EVENT_SPEAK, text="King: Welcome back. My Son, it's good to see you and your friends again...\nFaelon: Father, we have nearly achieved victory, but the staff must be destroyed!\nKing: Ahh... well, there is one way...\nEny: Tell us, we'll do anything!\nKing: Throw the staff into a volcano, it's the only way.\nMel: There is a live volcano East of here, through the jungle.\nKing: Aye, that is where you must go.\nKing: And you need to go soon... Our troops are battling monsters all over the continent.\nKing: You brave four must leave soon.\nEny: Thank you Sire, we will be on our way.\nKing: Do return! \\ Faelon: Yes my Father!\n" },
			{ event_type=EVENT_WAIT_FOR_SPEECH },
			{ event_type=EVENT_REST, delay=1000 },
			{ event_type=EVENT_CUSTOM, callback=done_third_scene },
		}
	end

	if (third_scene_started) then
		eny:update(step)
		if (not (faelon == nil)) then
			faelon:update(step)
			tig:update(step)
			mel:update(step)
		end
	end
end

function activate(activator, activated)
	if (activated == King.id) then
		if (getMilestone(MS_BEAT_TODE)) then
			doDialogue("King: Congratulations!\n")
		elseif (getMilestone(MS_BEAT_GIRL_DRAGON)) then
			doDialogue("King: A staff on the moon is certainly better than a staff on the Earth...\nHowever, no place is safe as long as the staff remains.\n")
		elseif (getMilestone(MS_BEAT_RIDER)) then
			doDialogue("King: Quick, to the volcano!\n")
		else
			doDialogue("King: Please, take your rest in the castle... stay as long as you like...\n")
		end
	elseif (not (faelon == nil) and activated == faelon.id) then
		doDialogue("Faelon: I'm glad you made it back! The turmoil is mostly over with here, thanks to you!\n")
	elseif (activated == door.id) then
		doDialogue("You can't go in there...\n")
	end
end

function collide(id1, id2)
end

