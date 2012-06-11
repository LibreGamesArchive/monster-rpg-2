function cb1(o, e)
	halt_rider_updates = true
	setObjectSubAnimation(rios.id, "stand_e")
	doDialogue("Rios: The entire library... destroyed.\n", true, false, true)
	setObjectSubAnimation(rios.id, "stand_s")
	doDialogue("Rios: Well I do have all the books memorized...\nRios: I just need a safe place to store the new collection.\nEny: We're soon heading to the Eastern Kingdom. The Kingdom MUST be safe...\nRios: I see... and could you do with the services of a Cleric?\nRider: I hope you know how to wield that staff in battle!\nRios: Sure, but healing is my specialty.\nEny: Then join us!\n", true, false, true)
	px, py = getObjectPosition(0)
	setObjectSolid(rios.id, false)
	rios.scripted_events = {
		{ event_type=EVENT_WALK, dest_x=px, dest_y=py },
	}
	addPartyMember("Rios")
	setMilestone(MS_TALKED_TO_RIOS, true)
	halt_rider_updates = false
end

function cb2(o, e)
	descriptifyPlayer()
	removeObject(rios.id)
	removeObject(rider.id)
	setMilestone(MS_RIOS_JOINED, true)
end

if (getMilestone(MS_SEASIDE_REPAIRED)) then
	music = "village.ogg"
else
	music = "burned_village.ogg"
end

function start()
	upPortal = Portal:new{x=13, y=3}
	if (not getMilestone(MS_RIOS_JOINED)) then
		rios = Object:new{x=2, y=3, anim_set="Rios", person=true}
		updateObject(rios.id, 1)
		setObjectSubAnimation(rios.id, "mope")
	end
end

function stop()
end

function update(step)
	if ((not getMilestone(MS_RIOS_JOINED)) and getMilestone(MS_ACTIVATED_RIOS)) then
		if (not halt_rider_updates) then
			rider:update(step)
		end
		if (getMilestone(MS_TALKED_TO_RIOS) and (not getMilestone(MS_RIOS_JOINED))) then
			rios:update(step)
		end
	end

	upPortal:update()

	if (upPortal.go_time) then
		change_areas("seaside", 28, 6, DIRECTION_SOUTH);
	end
end

function activate(activator, activated)
	if ((not getMilestone(MS_RIOS_JOINED)) and activated == rios.id) then
		setMilestone(MS_ACTIVATED_RIOS, true)
		px, py = getObjectPosition(0)
		rx, ry = getObjectPosition(rios.id)
		dx1, dy1, dx2, dy2 = 0, 0, 0, 0
		dir = DIRECTION_NORTH
		if (px > rx) then
			dx1, dy1 = 3, 4
			dx2, dy2 = 2, 4
		else
			dx1, dy1 = 3, 4
			dx2, dy2 = 3, 3
			dir = DIRECTION_WEST
		end
		rider = Object:new{x=px, y=py, anim_set="Rider", person=true}
		setObjectSolid(rider.id, false)
		rider.scripted_events = {
			{ event_type=EVENT_WALK, dest_x=dx1, dest_y=dy1 },
			{ event_type=EVENT_WALK, dest_x=dx2, dest_y=dy2 },
			{ event_type=EVENT_LOOK, direction=dir },
			{ event_type=EVENT_REST, delay=1000 },
			{ event_type=EVENT_CUSTOM, callback=cb1 },
			{ event_type=EVENT_WALK, dest_x=px, dest_y=py },
			{ event_type=EVENT_CUSTOM, callback=cb2 }
		}
		scriptifyPlayer()
	end
end

function collide(id1, id2)
end

