music = "keep.caf"

enemies = {}
enemies[0] = "3Stabbers"
enemies[1] = "2Stabbers2Halberds"
enemies[2] = "1Beast"
enemies[3] = "3Rages"

exceptions = {
	{3,6},
	{14,3},
	{24,6},
	{23,16},
	{24,18},
	{3,18},
	{13,6},
	{14,6},
}

scene_started = false

function done_scene()
	descriptifyPlayer()
	scene_started = false
	setMilestone(MS_KEEP_4_GOBLINS, true)
	for i=1,3 do
		removeObject(goblins[i].id)
	end
end


function start()
	down = Portal:new{x=14, y=3}
	uptower1 = Portal:new{x=24, y=6}
	uptower2 = Portal:new{x=24, y=18}
	uptower3 = Portal:new{x=3, y=18}
	uptower4 = Portal:new{x=3, y=6}
	up = Portal:new{x=23, y=16}

	if (not getMilestone(MS_KEEP_4_GOBLINS)) then
		goblins = {}
		goblins[1] = Object:new{x=13, y=10, anim_set="Goblin", person=true}
		goblins[2] = Object:new{x=14, y=10, anim_set="Goblin", person=true}
		goblins[3] = Object:new{x=15, y=10, anim_set="Goblin", person=true}
		setObjectDirection(goblins[1].id, DIRECTION_NORTH)
		setObjectDirection(goblins[2].id, DIRECTION_NORTH)
		setObjectDirection(goblins[3].id, DIRECTION_NORTH)
	end
end

function stop()
end

function update(step)
	check_battle(30, enemies, exceptions)

	if (down:update()) then
		change_areas("Keep_3", 14, 3, DIRECTION_SOUTH)
	elseif (uptower1:update()) then
		change_areas("Keep_t1-1", 3, 4, DIRECTION_SOUTH)
	elseif (uptower2:update()) then
		change_areas("Keep_t2-1", 3, 4, DIRECTION_SOUTH)
	elseif (uptower3:update()) then
		change_areas("Keep_t3-1", 3, 4, DIRECTION_SOUTH)
	elseif (uptower4:update()) then
		change_areas("Keep_t4-1", 3, 4, DIRECTION_SOUTH)
	elseif (up:update()) then
		change_areas("Keep_outer", 24, 9, DIRECTION_WEST)
	end

	if (not getMilestone(MS_KEEP_4_GOBLINS) and not scene_started) then
		local px, py = getObjectPosition(0)
		if ((px == 13 and py == 6) or (px == 14 and py == 6)) then
			scene_started = true
			stopObject(0)
			scriptifyPlayer()
			doDialogue("Goblin: Hey! Get outta here!\n", true)
			goblins[1].scripted_events = {
				{ event_type=EVENT_WALK, dest_x=13, dest_y=7 },
				{ event_type=EVENT_BATTLE, id="3Halberds", can_run=false },
				{ event_type=EVENT_WAIT_FOR_BATTLE },
				{ event_type=EVENT_CUSTOM, callback=done_scene },
			}
			goblins[2].scripted_events = {
				{ event_type=EVENT_WALK, dest_x=14, dest_y=7 },
				{ event_type=EVENT_SYNC, who=goblins[1], number=4 },
			}
			goblins[3].scripted_events = {
				{ event_type=EVENT_WALK, dest_x=15, dest_y=7 },
				{ event_type=EVENT_SYNC, who=goblins[1], number=4 },
			}
		end
	elseif (scene_started and not (goblins[1].scripted_events == nil)) then
		for i=1,3 do
			goblins[i]:update(step)
		end
	end
end

function activate(activator, activated)
end

function collide(id1, id2)
end

