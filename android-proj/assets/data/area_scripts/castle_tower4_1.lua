music = "castle.caf"

scene_started = false

function do_tint()
	set_music_volume(0)
	tint(60, 60, 60, 0)
	loadPlayDestroy("sleep.ogg")
end

function done()
	reviveAllPlayers()
	reverse_tint(60, 60, 60, 0)
	setObjectSolid(0, true)
	descriptifyPlayer()
	scene_started = false
	set_music_volume(1)
end

function start()
	exit = Portal:new{x=4, y=9, width=1, height=1}
	up = Portal:new{x=1, y=5, width=1, height=1}
        bed0 = Object:new{x=7, y=4}
        bed1 = Object:new{x=7, y=5}
end

function stop()
end

function update(step)
	exit:update()
	up:update()

	if (exit.go_time) then
		change_areas("castle", 30, 44, DIRECTION_SOUTH)
	end
	if (up.go_time) then
		change_areas("castle_tower4_2", 1, 5, DIRECTION_EAST)
	end

	if (scene_started) then
		eny:update(step)
	end

	if (not getMilestone(MS_CASTLE_SLEEP_HINT)) then
		px, py = getObjectPosition(0)
		if (py <= 6) then
			stopObject(0)
			setMilestone(MS_CASTLE_SLEEP_HINT, true)
			doDialogue("Eny: I could sleep here...\n")
		end
	end
end

function activate(activator, activated)
	if (activated == bed0.id or activated == bed1.id) then
		if (not scene_started) then
			scene_started = true
			px, py = getObjectPosition(0)
			eny = Object:new{id=0, x=px, y=py}
			scriptifyPlayer()
			setObjectSolid(0, false)
			eny.scripted_events = {
				{ event_type=EVENT_WALK, dest_x=7, dest_y=4 },
				{ event_type=EVENT_LOOK, direction=DIRECTION_SOUTH },
				{ event_type=EVENT_GESTURE, name="sleep" },
				{ event_type=EVENT_CUSTOM, callback=do_tint },
				{ event_type=EVENT_REST, delay=6000 },
				{ event_type=EVENT_WALK, dest_x=6, dest_y=4 },
				{ event_type=EVENT_LOOK, direction=DIRECTION_SOUTH },
				{ event_type=EVENT_CUSTOM, callback=done },
			}
		end
	end
end

function collide(id1, id2)
end

