music = "tivoli.caf"

local scene_started = false

function start()
	cow1 = Object:new{x=4, y=3, anim_set="cow", width=2}
	cow2 = Object:new{x=11, y=8, anim_set="cow", width=2}
	cow3 = Object:new{x=16, y=21, anim_set="cow", width=2}
	cow4 = Object:new{x=2, y=20, anim_set="cow", width=2}

	setObjectSubAnimation(cow1.id, "east")
	setObjectSubAnimation(cow2.id, "west")
	setObjectSubAnimation(cow3.id, "east")
	setObjectSubAnimation(cow4.id, "west")

	door = Door:new{x=27, y=23, anim_set="home_door"}
	hportal = Portal:new{x=27, y=23}

	outportal = Portal:new{x=0, y=18}

	if (getMilestone(MS_FARMER_GONE_TO_GATE)) then
		launch_portal = Portal:new{x=33, y=24}
		setTileSolid(32, 24, false)
		setTileLayer(32, 24, 2, -1)
		if (not getMilestone(MS_FARMER_AT_LAUNCHPAD)) then
			farmer = Object:new{x=32, y=24, anim_set="Farmer", person=true}
		end
	end
end

function stop()
end

function update(step)
	if (door == nil and hportal:update()) then
		change_areas("farmhouse", 7, 11, DIRECTION_NORTH)
	elseif (outportal:update()) then
		doMap("farm")
	end

	if (getMilestone(MS_FARMER_GONE_TO_GATE)) then
		if (launch_portal:update()) then
			change_areas("launchpad", 1, 7, DIRECTION_EAST)
		end
	end

	if (scene_started) then
		farmer:update(step)
	end
end

function remove_farmer()
	setMilestone(MS_FARMER_AT_LAUNCHPAD, true)
	scene_started = false
	descriptifyPlayer()
	removeObject(farmer.id)
end

function activate(activator, activated)
	if (activated == cow1.id) then
		doDialogue("Mmmooo...\n")
		setMilestone(MS_COW1, true)
	elseif (activated == cow2.id) then
		doDialogue("Mmmooo...\n")
		setMilestone(MS_COW2, true)
	elseif (activated == cow3.id) then
		doDialogue("Mmmooo...\n")
		setMilestone(MS_COW3, true)
	elseif (activated == cow4.id) then
		doDialogue("Mmmooo...\n")
		setMilestone(MS_COW4, true)
	elseif (farmer and farmer.id == activated) then
		scriptifyPlayer()
		setObjectDirection(farmer.id, player_dir(farmer))
		doDialogue("Farmer: Follow me fellers!\n", true)
		farmer.scripted_events = {
			{ event_type=EVENT_WALK, dest_x=33, dest_y=24 },
			{ event_type=EVENT_CUSTOM, callback=remove_farmer },
		}
		scene_started = true
	end
end

function collide(id1, id2)
	if ((not (door == nil)) and (id1 == door.id or id2 == door.id)) then
		door:open()
		removeObject(door.id)
		door = nil
	end
end

