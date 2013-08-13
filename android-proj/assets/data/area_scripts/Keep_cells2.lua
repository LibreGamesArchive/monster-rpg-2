music = "keep.ogg"

scene_started = false

enemies = {}
enemies[0] = "1DarkTroll"

exceptions = {
	{13,0},
	{14,0},
	{13,1},
	{14,1},
}

function prisoner_gone()
	descriptifyPlayer()
	setMilestone(MS_FREED_PRISONER, true)
	scene_started = false
	removeObject(prisoner.id)
end

function start()
	up = Portal:new{x=13, y=0, width=2}

	if (not getMilestone(MS_FREED_PRISONER)) then
		prisoner = Object:new{x=10, y=4, anim_set="prisoner", person=true}
		setObjectDirection(prisoner.id, DIRECTION_SOUTH)
	end

	switch = Object:new{x=12, y=7}

	cell_door = Door:new{x=10, y=7, anim_set="cell_door"}
	
	if (getMilestone(MS_CELL2_OPENED)) then
		cell_door:open(true)
		removeObject(cell_door.id)
		cell_door = nil
	end
end

function stop()
end

function update(step)
	check_battle(30, enemies, exceptions)

	if (up:update()) then
		change_areas("Keep_1", 4, 14, DIRECTION_EAST)
	end

	if (scene_started) then
		prisoner:update(step)
	end
end

function activate(activator, activated)
	if (activated == switch.id) then
		cell_door:open()
		removeObject(cell_door.id)
		cell_door = nil
		setMilestone(MS_CELL2_OPENED, true)
	end
	
	if (not getMilestone(MS_FREED_PRISONER)) then
		if (activated == prisoner.id) then
			setObjectDirection(prisoner.id, player_dir(prisoner))
			doDialogue("Prisoner: Bless your souls! Thanks for saving me!\n")
			setObjectSolid(prisoner.id, false)
			scriptifyPlayer()
			scene_started = true
			prisoner.scripted_events = {
				{ event_type=EVENT_WALK, dest_x=10, dest_y=8 },
				{ event_type=EVENT_WALK, dest_x=13, dest_y=8 },
				{ event_type=EVENT_WALK, dest_x=13, dest_y=1 },
				{ event_type=EVENT_CUSTOM, callback=prisoner_gone },
			}
		end
	end
end

function collide(id1, id2)
end

