music = "keep.caf"

PASSAGE_X = 14
PASSAGE_Y = 3
can_squeek = true
step_count = 0

function done()
	setMilestone(MS_CELL_SCENE, true)
end

enemies = {}
enemies[0] = "1DarkTroll"

exceptions = {
	{1,0},
	{2,0},
	{1,1},
	{2,1},
}

function start()
	switch = Object:new{x=11, y=7}
	cell_door = Door:new{x=13, y=7, anim_set="cell_door"}
	if (getMilestone(MS_ENY_CELL_OPENED)) then
		cell_door:open(true)
		removeObject(cell_door.id)
		cell_door = nil
	end

	chest = Chest:new{x=14, y=10, anim_set="chest", milestone=MS_KEEP_CHEST_0, index=ITEM_ONYX_CAP }
	if (not getMilestone(MS_CELL_SCENE)) then
		eny = Object:new{id=0, x=13, y=4}
		goblin = Object:new{x=10, y=10, anim_set="Goblin" }
		setObjectSubAnimation(goblin.id, "passed_out")
		eny.scripted_events = {
			{ event_type=EVENT_REST, delay=5000 },
			{ event_type=EVENT_GESTURE, name="hurt" },
			{ event_type=EVENT_REST, delay=3000 },
			{ event_type=EVENT_GESTURE, name="stand_s" },
			{ event_type=EVENT_SPEAK, text="Ohh, my head. This is bad! I've got to save Tiggy from that evil staff, and I'll probably need all the help I can get to do so!\nFirst things first, how do I get out of here?\n" },
			{ event_type=EVENT_DESCRIPTIFY },
			{ event_type=EVENT_CUSTOM, callback=done },
		}
	end
	
	if (getMilestone(MS_REMOVED_SQUEEKY_BOARDS)) then
		exit = Portal:new{x=PASSAGE_X, y=PASSAGE_Y}
	else
		passage = Object:new{x=PASSAGE_X, y=PASSAGE_Y, anim_set="hidden_passage"}
		setObjectSolid(passage.id, false)
	end

	up = Portal:new{x=1, y=0, width=2}
end

function stop()
end

function update(step)
	if (getMilestone(MS_BEAT_TIGGY)) then
		check_battle(30, enemies, exceptions)
	end

	if (not getMilestone(MS_CELL_SCENE)) then
		eny:update(step)
	end
	if (not getMilestone(MS_REMOVED_SQUEEKY_BOARDS)) then
		px, py = getObjectPosition(0)
		if (px == PASSAGE_X and py == PASSAGE_Y) then
			if (can_squeek) then
				can_squeek = false
				step_count = step_count + 1
				if (step_count >= 3) then
					doDialogue("Eny: I need to stand back and lift these boards...\n")
				else
					doDialogue("* squeek *\n", false)
				end
			end
		else
			can_squeek = true
		end
	else
		exit:update()

		if (exit.go_time) then
			change_areas("Underground_start", 1, 5, DIRECTION_EAST);
		end
	end

	if (up:update()) then
		change_areas("Keep_1", 33, 14, DIRECTION_WEST)
	end
end

function activate(activator, activated)
	if (not getMilestone(MS_REMOVED_SQUEEKY_BOARDS)) then
		if (activated == passage.id) then
			doDialogue("Hm, these boards look loose...\n", true)
			loadPlayDestroy("chest.ogg")
			setMilestone(MS_REMOVED_SQUEEKY_BOARDS, true)
			exit = Portal:new{x=PASSAGE_X, y=PASSAGE_Y}
			removeObject(passage.id)
		end
	end

	if (activated == chest.id) then
		chest:activate()
	end
	
	if (activated == switch.id) then
		cell_door:open()
		removeObject(cell_door.id)
		cell_door = nil
		setMilestone(MS_ENY_CELL_OPENED, true);
	end
end

function collide(id1, id2)
end

