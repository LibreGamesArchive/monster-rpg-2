music = "shyzu.ogg"

scene_started = false

function start()
	in1 = Portal:new{x=8, y=8}
	in2 = Portal:new{x=18, y=10}
	in3 = Portal:new{x=8, y=16}
	in4 = Portal:new{x=21, y=18}
	in5 = Portal:new{x=28, y=24}
	out = Portal:new{x=0, y=7, width=1, height=2}
	out2 = Portal:new{x=39, y=14, width=1, height=2}

	sign = Object:new{x=15, y=10}
	male = Object:new{x=10, y=21, anim_set="shyzu_male", person=true, move_type=MOVE_WANDER}

	if (not getMilestone(MS_TIPPER_JOINED)) then
		tipper = Object:new{x=29, y=11, anim_set="Tipper", person=true, move_type=MOVE_WANDER}
	end
end

function stop()
end

function update(step)
	if (in1:update()) then
		change_areas("shyzu_in-1", 2, 6, DIRECTION_NORTH)
	elseif (in2:update()) then
		change_areas("shyzu_in-2", 2, 6, DIRECTION_NORTH)
	elseif (in3:update()) then
		change_areas("shyzu_in-3", 2, 6, DIRECTION_NORTH)
	elseif (in4:update()) then
		change_areas("shyzu_in-4", 2, 6, DIRECTION_NORTH)
	elseif (in5:update()) then
		change_areas("shyzu_in-5", 2, 6, DIRECTION_NORTH)
	elseif (out:update() or out2:update()) then
		doMap("shyzu", "map2")
	end

	male:move(step)

	if ((not scene_started) and (not getMilestone(MS_TIPPER_JOINED))) then
		tipper:move(step)
	elseif (scene_started and (not getMilestone(MS_TIPPER_JOINED))) then
		tipper:update(step)
	end
end

function done()
	removeObject(tipper.id)
	addPartyMember("Tipper")
	descriptifyPlayer()
	scene_started = false
	setMilestone(MS_TIPPER_JOINED, true)
end

function activate(activator, activated)
	if (activated == sign.id) then
		doDialogue("\"Home of the world famous spicy meatball!\"\n")
	elseif (activated == male.id) then
		setObjectDirection(male.id, player_dir(male))
		doDialogue("Male: Hi, how are you?\n")
	elseif (tipper and activated == tipper.id) then
		stopObject(0)
		scriptifyPlayer()
		setObjectSolid(tipper.id, false)
		setObjectDirection(tipper.id, player_dir(tipper))
		local px, py = getObjectPosition(0)
		doDialogue("Tipper: Oh yeah, I saw that meteor or whatever it was... landed North of here.\nTipper: I bet the King's men tracked it down.\nTipper: ... Sure I'll show you the way. I'm always up for adventure!\n", true)
		tipper.scripted_events = {
			{ event_type=EVENT_WALK, dest_x=px, dest_y=py },
			{ event_type=EVENT_CUSTOM, callback=done },
		}
		scene_started = true
	end
end

function collide(id1, id2)
end

