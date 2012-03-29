music = "keep.caf"

guard_battle_started = false
troll_battle_started = false
troll_dialogue_started = false
hack_count = 0
nanner_battle_started = false
rider_battle_started = false
done_archery_already = false
done_look_already = false

function done_goblin_walk()
	removeObject(goblin.id)
	goblin = nil
	goblin_walk_started = false
end

function start()
	door_enter1 = Portal:new{x=18, y=32}
	door_enter2 = Portal:new{x=19, y=32}
	door_enter = Door:new{x=17, y=32, anim_set="Keep_main_door", width=4}
	bigtowdown1 = Portal:new{x=6, y=24}
	bigtowdown2 = Portal:new{x=31, y=24}
	topdown = Portal:new{x=24, y=9}
	mid_door = Door:new{x=18, y=20, width=2, anim_set="Keep_mid_door"}
	mid_portal = Portal:new{x=18, y=20, width=2}

	if (not getMilestone(MS_BEAT_KEEP_DOOR_GUARDS)) then
		guard1 = Object:new{x=18, y=33, anim_set="Goblin", person=true}
		guard2 = Object:new{x=19, y=33, anim_set="Goblin", person=true}
		setObjectDirection(guard1.id, DIRECTION_SOUTH)
		setObjectDirection(guard2.id, DIRECTION_SOUTH)
	end

	if (not getMilestone(MS_BEAT_TROLL)) then
		troll = Object:new{x=31, y=23, anim_set="Troll"}
	end

	if (not getMilestone(MS_KILLED_NANNER)) then
		nanner = Object:new{x=28, y=8, anim_set="Nanner"}
	else
		chest = Chest:new{x=28, y=8, anim_set="chest", milestone=MS_NANNER_CHEST, index=ITEM_KEEP_KEY_UP}
	end

	if (not getMilestone(MS_BEAT_RIDER)) then
		rider = Object:new{x=18, y=7, anim_set="Rider", person=true}
		setObjectIsFloater(rider.id, true)
		setObjectAnimationSetPrefix(rider.id, "staff_")
		setObjectDirection(rider.id, DIRECTION_SOUTH)
	end

	if (getMilestone(MS_HELPED_GOBLIN) and not getMilestone(MS_TALKED_TO_HELPED_GOBLIN)) then
		goblin = Object:new{x=24, y=8, anim_set="Goblin", person=true}
		setObjectDirection(goblin.id, DIRECTION_SOUTH)
	end
	
	if (getMilestone(MS_BEAT_RIDER) and not getMilestone(MS_GOT_STAFF)) then
		staff = Chest:new{x=18, y=7, anim_set="staff", milestone=MS_GOT_STAFF, index=ITEM_STAFF}
	end
	

	if (getMilestone(MS_GONE_TO_ARCHERY_TOWER)) then
		--dpad_off()
	end
end

function stop()
end

local loops = 0

function update(step)
	if (loops < 1) then
		loops = loops + 1
		return
	end

	if (not getMilestone(MS_BEAT_KEEP_DOOR_GUARDS) and guard_battle_started) then
		if (not inBattle() and battleWon()) then
			setMilestone(MS_BEAT_KEEP_DOOR_GUARDS, true)
			removeObject(guard1.id)
			removeObject(guard2.id)
		end
	end


	if (getMilestone(MS_GONE_TO_ARCHERY_TOWER) and not done_archery_already) then
		clearBuffer()
		done_archery_already = true
		local for_points = (not getMilestone(MS_BEAT_ARCHERY))
		local doit = true;
		if (not for_points) then
			doit = prompt("Shoot up", "some baddies?", 0, 0)
			clearBuffer()
		end
		if (doit) then
			local b = doArchery(for_points)
			if (b) then
				setMilestone(MS_BEAT_ARCHERY, true)
				playMusic("keep.caf")
			else
				gameOver()
			end
		end
		-- ?
		--dpad_on()
		updateArea()
		drawArea()
		transitionIn()
	elseif (not CRAP) then
		CRAP = true
	elseif (getMilestone(MS_GONE_TO_ARCHERY_TOWER) and not done_look_already) then
		done_look_already = true
		local should_look = prompt("Take a look", "around?", 0, 1)
		if (should_look) then
			doKeepLook()
			setMilestone(MS_LOOKED_FROM_KEEP, true)
			doDialogue("Eny: The view from here is breathtaking!\n")
		end
	end

	if (troll_battle_started) then
		if (not inBattle()) then
			troll_battle_started = false
			if (battleWon()) then
				descriptifyPlayer()
				removeObject(troll.id)
				setMilestone(MS_BEAT_TROLL, true)
			end
		end
	elseif (hack_count > 0 and not troll_dialogue_started and getMilestone(MS_GONE_TO_TROLL_TOWER) and not getMilestone(MS_BEAT_TROLL)) then
		troll_dialogue_started = true
		stopObject(0)
		setObjectDirection(0, DIRECTION_NORTH)
		scriptifyPlayer()
		doDialogue("Troll: Neeeeeeuuu!\nEny: What are you saying?\nTroll: Naaaaaaaaaarrr!\n", true, false, true)
		startBattle("1Troll", true)
		troll_battle_started = true
	end

	hack_count = hack_count + 1

	px, py = getObjectPosition(0)
	if (py > 62) then
		stopObject(0)
		doMap("keep")
	end

	if (door_enter1:update()) then
		change_areas("Keep_1", 18, 25, DIRECTION_NORTH)
	elseif (door_enter2:update()) then
		change_areas("Keep_1", 19, 25, DIRECTION_NORTH)
	elseif (bigtowdown1:update()) then
		setMilestone(MS_GONE_TO_ARCHERY_TOWER, false)
		change_areas("Keep_2", 3, 23, DIRECTION_EAST)
	elseif (bigtowdown2:update()) then
		setMilestone(MS_GONE_TO_TROLL_TOWER, false)
		change_areas("Keep_2", 34, 23, DIRECTION_WEST)
	elseif (topdown:update()) then
		change_areas("Keep_4", 23, 16, DIRECTION_WEST)
	elseif (mid_portal:update()) then
		change_areas("Keep_3", 14, 17, DIRECTION_NORTH)
	end

	if (nanner_battle_started) then
		if (not inBattle()) then
			nanner_battle_started = false
			descriptifyPlayer()
			if (battleWon()) then
				setMilestone(MS_KILLED_NANNER, true)
				chest = Chest:new{x=28, y=8, anim_set="chest", milestone=MS_NANNER_CHEST, index=ITEM_KEEP_KEY_UP}
				removeObject(nanner.id)
			end
		end
	end

	if (rider_battle_started) then
		if (not inBattle()) then
			rider_battle_started = false
			if (battleWon()) then
				setMilestone(MS_BEAT_RIDER, true)
				removeObject(rider.id)
				if (not (goblin == nil)) then
					removeObject(goblin.id)
					goblin = nil
				end
				staff = Chest:new{x=18, y=7, anim_set="staff", milestone=MS_GOT_STAFF, index=ITEM_STAFF}
			end
		end
	end

	if (goblin_walk_started) then
		goblin:update(step)
	end
end

function activate(activator, activated)
	if (not getMilestone(MS_BEAT_KEEP_DOOR_GUARDS)) then
		if (activated == guard1.id or activated == guard2.id) then
			doDialogue("Goblin 1: Say, it's the girl who escaped!\nGoblin 2: Yaa, and she's brought some friends!\n", true)
			guard_battle_started = true
			startBattle("2Stabbers", true)
		end
	end

	if (not getMilestone(MS_KILLED_NANNER)) then
		if (activated == nanner.id) then
			scriptifyPlayer()
			doDialogue("Nanner: See what the Rider did to me?!...\nNanner: All I have left is this key, so no, you can't have it!\n", true, false, true)
			rest(0.5)
			doDialogue("Nanner: Hmm.. and I'm going to destroy you too!\n", true)
			startBattle("1Nanner", true)
			nanner_battle_started = true
		end
	end

	if (not (chest == nil)) then
		if (activated == chest.id) then
			chest:activate()
		end
	end
	
	if (not getMilestone(MS_BEAT_RIDER)) then
		if (activated == rider.id) then
			doDialogue("Rider: I underestimated you and I shouldn't have... I saw how you fight so I should have known.\nRider: You remember me? Haha...\nEny: That staff is destroying everything! You are summoning monsters into a peaceful world!\nRider: That is my destiny...\nEny: You have the power to change your destiny! Destroy the staff!\nRider: It's a part of me now, that cannot change...\nRider: It almost saddens me that I have to do this, but...\n", true)
			startBattle("RiderAndMinions", true)
			rider_battle_started = true
		end
	end

	if (getMilestone(MS_HELPED_GOBLIN)) then
		if ((not (goblin == nil)) and activated == goblin.id) then
			setObjectSolid(goblin.id, false)
			doDialogue("Goblin: Since you helped me I'm going to tell you a secret...\nMinions don't like their master, but they normally don't like you much either!\n", true)
			goblin_walk_started = true
			setMilestone(MS_TALKED_TO_HELPED_GOBLIN, true)
			goblin.scripted_events = {
				{ event_type=EVENT_WALK, dest_x=24, dest_y=9 },
				{ event_type=EVENT_CUSTOM, callback=done_goblin_walk },
			}
		end
	end

	if (getMilestone(MS_BEAT_RIDER)) then
		if ((not (staff == nil)) and activated == staff.id) then
			staff:activate()
			removeObject(staff.id)
		end
	end
end

function collide(id1, id2)
	if ((not (door_enter == nil)) and (id1 == door_enter.id or id2 == door_enter.id)) then
		door_enter:open()
		removeObject(door_enter.id)
		door_enter = nil
	elseif ((not (mid_door == nil)) and (id1 == mid_door.id or id2 == mid_door.id)) then
		mid_door:open()
		removeObject(mid_door.id)
		mid_door = nil
	end
end

