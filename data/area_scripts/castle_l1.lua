music = "castle.ogg"

local clicked_gunnar = false

function gunnar_cb()
	removeObject(gunnar.id)
	addPartyMember("Gunnar")
	setMilestone(MS_GUNNAR_JOINED, true)
	descriptifyPlayer()
end

function scriptify_cb()
	scriptifyPlayer()
end

function preparing_cb()
	done_preparing = true
end

function start()
	mainPortal = Portal:new{x=17, y=34, width=2, height=1}
	backPortal1 = Portal:new{x=9, y=16, width=1, height=3}
	backPortal2 = Portal:new{x=25, y=16, width=1, height=3}
	upLeftPortal = Portal:new{x=11, y=31, width=1, height=1}
	upRightPortal = Portal:new{x=23, y=31, width=1, height=1}
	leftSidePortal = Portal:new{x=0, y=25, width=1, height=1}
	rightSidePortal = Portal:new{x=34, y=25, width=1, height=1}

	if (not getMilestone(MS_BEACH_BATTLE_DONE)) then
		soldier1 = Object:new{x=22, y=45, anim_set="soldier", person=true, move_type=MOVE_NONE}
		updateArea();
		setObjectDirection(soldier1.id, DIRECTION_EAST)
		setObjectSubAnimation(soldier1.id, "drink")
		soldier2 = Object:new{x=23, y=46, anim_set="soldier", person=true}
		setObjectDirection(soldier2.id, DIRECTION_NORTH)
		soldier3 = Object:new{x=25, y=45, anim_set="soldier", person=true}
		setObjectDirection(soldier3.id, DIRECTION_WEST)
	end

	chef = Object:new{x=19, y=17, anim_set="chef", person=true}
	updateArea()
	setObjectSubAnimation(chef.id, "prepare")
	
	if (not getMilestone(MS_GUNNAR_JOINED)) then
		gunnar = Object:new{x=13, y=16, anim_set="Gunnar", person=true, dx1=13, dy1=16, dx2=14, dy2=16, dir1=DIRECTION_SOUTH, dir2=DIRECTION_NORTH, pause=5000, move_type=MOVE_LINE, rest=0}
	end
end

function stop()
end

function update(step)
	if (clicked_gunnar and not getMilestone(MS_GUNNAR_JOINED)) then
		gunnar:update(step)
	end

	mainPortal:update()
	
	if (mainPortal.go_time) then
		change_areas("castle", 22, 33, DIRECTION_SOUTH)
	end

	backPortal1:update()

	if (backPortal1.go_time) then
		change_areas("castle", 18, 24, DIRECTION_NORTH)
	end

	backPortal2:update()

	if (backPortal2.go_time) then
		change_areas("castle", 27, 24, DIRECTION_NORTH)
	end

	upLeftPortal:update()

	if (upLeftPortal.go_time) then
		change_areas("castle_l2", 3, 18, DIRECTION_NORTH) 
	end

	upRightPortal:update()

	if (upRightPortal.go_time) then
		change_areas("castle_l2", 15, 18, DIRECTION_NORTH) 
	end

	leftSidePortal:update()
	rightSidePortal:update()

	if (leftSidePortal.go_time) then
		change_areas("castle", 12, 31, DIRECTION_WEST)
	end
	if (rightSidePortal.go_time) then
		change_areas("castle", 33, 31, DIRECTION_EAST)
	end

	if (preparing) then
		chef:update(step)
	end

	if (done_preparing) then
		preparing = false
		done_preparing = false
		setObjectDirection(chef.id, player_dir(chef))
		updateArea()
		doDialogue("Chef: Here you go!\n", true, false, true)
		loadPlayDestroy("chest.ogg")
		setInventory(slot, ITEM_JUICE, 1)
		setObjectSubAnimation(chef.id, "prepare")
		setMilestone(MS_GOT_FRUIT, false)
		descriptifyPlayer()
	end

	if (not getMilestone(MS_GUNNAR_JOINED)) then
		if (not (gunnar == nil)) then
			gunnar:move(step)
		end
	end
end

function activate(activator, activated)
	if (not getMilestone(MS_BEACH_BATTLE_DONE)) then
		if (activated == soldier1.id) then
			setObjectDirection(soldier1.id, player_dir(soldier1))
			doDialogue("Soldier: Have something from the kitchen!\n", true)
			setObjectSubAnimation(soldier1.id, "drink")
		elseif (activated == soldier2.id) then
			setObjectDirection(soldier2.id, player_dir(soldier2))
			smartDialogue{"Soldier: We're on break right now...\n",
				MS_BEACH_BATTLE_DONE, "Soldier: I've never faced anything in battle before!\n"}
			setObjectDirection(soldier2.id, DIRECTION_NORTH)
		elseif (activated == soldier3.id) then
			setObjectDirection(soldier3.id, player_dir(soldier3))
			smartDialogue{"Soldier: It's peaceful but we still train hard!\n",
				MS_BEACH_BATTLE_DONE, "Soldier: We need to step up training a notch!...\n"}
			setObjectDirection(soldier3.id, DIRECTION_WEST)
		end
	end

	if (activated == chef.id and not lock) then
		lock = true
		setObjectDirection(chef.id, player_dir(chef))
		updateArea()
		slot = findUsedInventorySlot(ITEM_JUICE)
		if (slot >= 0) then
			doDialogue("Chef: Finish the juice you have first, then I'll make you some more...\n", true)
			setObjectSubAnimation(chef.id, "prepare")
		else
			slot = findUsedInventorySlot(ITEM_FRUIT)
			if (slot >= 0) then
				doDialogue("Chef: Let me make some fruit juice for you!\n", true)
				chef.scripted_events = {
					{ event_type=EVENT_CUSTOM, callback=scriptify_cb },
					{ event_type=EVENT_GESTURE, name="prepare" },
					{ event_type=EVENT_REST, delay=4000 },
					{ event_type=EVENT_CUSTOM, callback=preparing_cb },
				}
				preparing = true
			else
				doDialogue("Chef: I will prepare whatever you want, but you must bring me the supplies.\n", true)
				setObjectSubAnimation(chef.id, "prepare")
			end
		end
		lock = false
	elseif (activated == gunnar.id) then
		setObjectDirection(gunnar.id, player_dir(gunnar))
		scriptifyPlayer()
		doDialogue("Gunnar: What am I doing? I design machines for the King.\nEny: We hear there are monsters attacking Flowey. Do you know if it's true?\nGunnar: I've heard the rumors, but I didn't think there was any way!\nEny: Monsters have already burned down Seaside, they have been awakened.\nEny: My friend became possessed by a staff with evil powers.\nGunnar: Then I must get to Flowey, and protect my place of birth!\nEny: Join us, then!\nGunnar: The path is through the mountains!\n", true)

		local px, py = getObjectPosition(0)
		setObjectSolid(gunnar.id, false)
		gunnar.scripted_events = {
			{ event_type=EVENT_WALK, dest_x=px, dest_y=py },
			{ event_type=EVENT_CUSTOM, callback=gunnar_cb }
		}
		clicked_gunnar = true
	end
end

function collide(id1, id2)
end

