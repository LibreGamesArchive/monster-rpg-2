music = "keep.caf"

hurt_goblin_scene_started = false
fight_scene_started = false

enemies = {}
enemies[0] = "3Stabbers"
enemies[1] = "2Stabbers2Halberds"

exceptions = {
	{19,3},
	{4,14},
	{33,14},
	{18,25},
	{19,25},
	{18,7},
	{19,7},
}


function spawn2()
	local n = 2
	goblins[n] = Object:new{x=18, y=3, anim_set="Goblin", person=true}
	setObjectDirection(goblins[2].id, DIRECTION_SOUTH)
	setObjectSolid(goblins[n].id, false)
	goblins[n].scripted_events = {
		{ event_type=EVENT_SYNC, who=goblins[1], number=8 },
		{ event_type=EVENT_REST, delay=1000 },
		{ event_type=EVENT_WALK, dest_x=18, dest_y=5 },
		{ event_type=EVENT_SYNC, who=goblins[1], number=12 },
	}
end

function spawn3()
	local n = 3
	goblins[n] = Object:new{x=19, y=4, anim_set="Goblin", person=true}
	setObjectDirection(goblins[2].id, DIRECTION_SOUTH)
	setObjectSolid(goblins[n].id, false)
	goblins[n].scripted_events = {
		{ event_type=EVENT_SYNC, who=goblins[1], number=8 },
		{ event_type=EVENT_WALK, dest_x=19, dest_y=6 },
		{ event_type=EVENT_SYNC, who=goblins[1], number=12 },
	}
end

function spawn4()
	local n = 4
	goblins[n] = Object:new{x=18, y=4, anim_set="Goblin", person=true}
	setObjectDirection(goblins[2].id, DIRECTION_SOUTH)
	setObjectSolid(goblins[n].id, false)
	goblins[n].scripted_events = {
		{ event_type=EVENT_SYNC, who=goblins[1], number=8 },
		{ event_type=EVENT_WALK, dest_x=18, dest_y=6 },
		{ event_type=EVENT_SYNC, who=goblins[1], number=12 },
	}
end

function fight_scene_done()
	descriptifyPlayer()
	for i=1,#goblins do
		removeObject(goblins[i].id)
	end
	fight_scene_started = false
	setMilestone(MS_KEEP_1_FIGHT, true)
end

function hurt_goblin_scene_done()
	hurt_goblin_scene_started = false
	descriptifyPlayer()
	removeObject(goblin.id)
	setMilestone(MS_HELPED_GOBLIN, true)
end

function start()
	out1 = Portal:new{x=18, y=25}
	out2 = Portal:new{x=19, y=25}
	up = Portal:new{x=19, y=3}
	down1 = Portal:new{x=4, y=14}
	down2 = Portal:new{x=33, y=14}

	chest0 = Chest:new{x=3, y=23, anim_set="chest", milestone=MS_KEEP_1_CHEST_0, itemtype=ITEM_GOLD, quantity=200}
	chest1 = Chest:new{x=34, y=23, anim_set="chest", milestone=MS_KEEP_1_CHEST_1, index=ITEM_ONYX_SWORD}
	chest2 = Chest:new{x=5, y=4, anim_set="chest", milestone=MS_KEEP_1_CHEST_2, index=ITEM_MYSTIC_HELMET}
	chest3 = Chest:new{x=6, y=4, anim_set="chest", milestone=MS_KEEP_1_CHEST_3, index=ITEM_MYSTIC_BOOTS}

	if (not getMilestone(MS_HELPED_GOBLIN) and not getMilestone(MS_BEAT_RIDER)) then
		goblin = Object:new{x=29, y=7, anim_set="Goblin", person=true}
		updateArea()
		setObjectSubAnimation(goblin.id, "dead")
	end
end

function stop()
end

function update(step)
	check_battle(30, enemies, exceptions)

	if (out1:update()) then
		change_areas("Keep_outer", 18, 33, DIRECTION_SOUTH)
	elseif (out2:update()) then
		change_areas("Keep_outer", 19, 33, DIRECTION_SOUTH)
	elseif (up:update()) then
		change_areas("Keep_2", 19, 3, DIRECTION_SOUTH)
	elseif (down1:update()) then	
		change_areas("Keep_cells2", 14, 0, DIRECTION_SOUTH)
	elseif (down2:update()) then	
		change_areas("Keep_cells", 1, 0, DIRECTION_SOUTH)
	end
	
	if (not getMilestone(MS_HELPED_GOBLIN) and not getMilestone(MS_BEAT_RIDER) and hurt_goblin_scene_started) then
		goblin:update(step)
	end

	if (not getMilestone(MS_KEEP_1_FIGHT) and fight_scene_started) then
		for i=1,#goblins do
			goblins[i]:update(step)
		end
	end
	
	if (not getMilestone(MS_KEEP_1_FIGHT) and not fight_scene_started) then
		local px, py = getObjectPosition(0)
		if ((px == 18 and py == 7) or (px == 19 and py == 7)) then
			stopObject(0)
			scriptifyPlayer()
			setObjectDirection(0, DIRECTION_NORTH)
			fight_scene_started = true
			goblins = {}
			goblins[1] = Object:new{x=19, y=3, anim_set="Goblin", person=true}
			setObjectDirection(goblins[1].id, DIRECTION_SOUTH)
			setObjectSolid(goblins[1].id, false)
			goblins[1].scripted_events = {
				{ event_type=EVENT_REST, delay=1000 },
				{ event_type=EVENT_CUSTOM, callback=spawn2 },
				{ event_type=EVENT_REST, delay=1000 },
				{ event_type=EVENT_CUSTOM, callback=spawn3 },
				{ event_type=EVENT_REST, delay=1000 },
				{ event_type=EVENT_CUSTOM, callback=spawn4 },
				{ event_type=EVENT_SPEAK, text="Goblin: There they are! \\ Let's get 'em!\n" },
				{ event_type=EVENT_WAIT_FOR_SPEECH },
				{ event_type=EVENT_REST, delay=1000 },
				{ event_type=EVENT_WALK, dest_x=19, dest_y=5 },
				{ event_type=EVENT_BATTLE, id="4Stabbers", can_run=false },
				{ event_type=EVENT_WAIT_FOR_BATTLE },
				{ event_type=EVENT_CUSTOM, callback=fight_scene_done },

			}
		end
	end
end

function activate(activator, activated)
	if (activated == chest0.id) then
		chest0:activate()
	elseif (activated == chest1.id) then
		chest1:activate()
	elseif (activated == chest2.id) then
		chest2:activate()
	elseif (activated == chest3.id) then
		chest3:activate()
	end
	
	if (not getMilestone(MS_HELPED_GOBLIN) and not getMilestone(MS_BEAT_RIDER)) then
		if (activated == goblin.id) then
			dir = getObjectDirection(0)
			scriptifyPlayer()
			setObjectDirection(0, dir)
			addSparklySpiralAroundObject(goblin.id)
			loadPlayDestroy("Revive.ogg")
			setObjectSubAnimation(goblin.id, "stand_s")
			doDialogue("Goblin: Yarp!\nGoblin: ...\nGoblin: One of the trolls went on a rampage!\nGoblin: Let's just say I owe you one!\n", true)
			hurt_goblin_scene_started = true
			setObjectSolid(goblin.id, false)
			goblin.scripted_events = {
				{ event_type=EVENT_WALK, dest_x=29, dest_y=8 },
				{ event_type=EVENT_WALK, dest_x=19, dest_y=8 },
				{ event_type=EVENT_WALK, dest_x=19, dest_y=3 },
				{ event_type=EVENT_CUSTOM, callback=hurt_goblin_scene_done },
			}
		end
	end
end

function collide(id1, id2)
end

