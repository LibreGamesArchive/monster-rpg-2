music = "Flowey.caf"

chase_started = false

TIMER_TOTAL = 180

golems_beaten = 0
had_battle = false
started_battle = false
fought_id = 0



doorways = {
	{ 8, 12 },
	{ 16, 12 },
	{ 13, 19 },
	{ 22, 21 },
	{ 13, 28 },
	{ 21, 27 },
}


function in_doorway(px, py)
	for i, v in ipairs(doorways) do
		if (v[1] == px and v[2] == py) then
			return true
		end
	end

	return false
end


function done()
	chase_started = true
	setTimer(TIMER_TOTAL, true)
	setMilestone(MS_GOLEM_TIMER_STARTED, true)
	descriptifyPlayer()
	playMusic("chase.caf")
end


function add_portals()
	inn = Portal:new{x=8, y=12}
	lib = Portal:new{x=13, y=19}
	mon = Portal:new{x=16, y=12}
	barr = Portal:new{x=13, y=28}
	shop = Portal:new{x=21, y=27}
	tav = Portal:new{x=22, y=21}
end

function add_gate(sound)
	if (sound) then
		loadPlayDestroy("door.ogg")
	end
	gate = Door:new{x=7, y=31, anim_set="gate"}
end

function remove_gate()
	loadPlayDestroy("door.ogg")
	removeObject(gate.id)
	gate = nil
end

function start()
	if (not getMilestone(MS_GOLEM_TIMER_STARTED)) then
		scriptifyPlayer()
		eny = Object:new{id=0, x=7, y=32}
		eny.scripted_events = {
			{ event_type=EVENT_WALK, dest_x=7, dest_y=29 },
			{ event_type=EVENT_GESTURE, name="stand_w" },
			{ event_type=EVENT_REST, delay=1000 },
			{ event_type=EVENT_GESTURE, name="stand_e" },
			{ event_type=EVENT_SPEAK, text="Guard: Am I ever glad to see you! There are two Golems breaking through the walls at the north of the village!\nGuard: Most of our troops are defending the beach front.\nEny: What can we do?\nGuard: Defeat the Golems, without walls the village will fall! And better make it quick!\n" },
			{ event_type=EVENT_WAIT_FOR_SPEECH },
			{ event_type=EVENT_CUSTOM, callback=done }
		}
		golem0 = Object:new{x=15, y=2, anim_set="Golem"}
		golem1 = Object:new{x=19, y=2, anim_set="Golem"}
		setObjectSubAnimation(golem0.id, "punch")
		setObjectSubAnimation(golem1.id, "punch")
	else
		setMilestone(MS_FLOWEY_WALLS_FIXED, true)
		add_portals()
	end

	add_gate(false)

	guard0 = Object:new{x=6, y=29, anim_set="Guard"}
	guard1 = Object:new{x=8, y=29, anim_set="Guard"}
	guard2 = Object:new{x=14, y=4, anim_set="Guard"}
	guard3 = Object:new{x=20, y=4, anim_set="Guard"}
	setObjectDirection(guard0.id, DIRECTION_SOUTH)
	setObjectDirection(guard1.id, DIRECTION_SOUTH)
	setObjectDirection(guard2.id, DIRECTION_SOUTH)
	setObjectDirection(guard3.id, DIRECTION_SOUTH)

	wall0 = Object:new{x=15, y=3, anim_set="clobbered_walls"}
	wall1 = Object:new{x=19, y=3, anim_set="clobbered_walls"}
	setObjectDimensions(wall0.id, 16, 32)
	setObjectDimensions(wall1.id, 16, 32)
	fake0 = Object:new{x=15, y=2}
	fake1 = Object:new{x=19, y=2}
end

function stop()
end

gate_open_x, gate_open_y = -1, -1

function update(step)
	if (started_battle and battleWon()) then
		started_battle = false
		had_battle = true
	end

	if (had_battle) then
		had_battle = false
		golems_beaten = golems_beaten + 1
		if (golems_beaten == 1) then
			--setMilestone(MS_KILLED_1_GOLEM, true)
			removeObject(fought_id)
		else
			setMilestone(MS_KILLED_GOLEMS, true)
			removeObject(fought_id)
			playMusic("Flowey.caf")
			add_portals()
		end
	end

	px, py = getObjectPosition(0)

	if (not (px == gate_open_x) or not (py == gate_open_y)) then
		can_close_gate = true
	end


	if (gate == nil and can_close_gate and ((px == 7 and py == 30) or (px == 7 and py == 32))) then
		add_gate(true)
		gate_open_x, gate_open_y = -1, -1
	end

	if (not getMilestone(MS_GOLEM_TIMER_STARTED)) then
		eny:update(step)
	end

	if (chase_started and not getMilestone(MS_KILLED_GOLEMS)) then
		local timer_time = TIMER_TOTAL - getTimer()
		local s = TIMER_TOTAL / 4

		if (timer_time > (s*3)) then
			if (golems_beaten == 1) then
				if (fought_id == golem0.id) then
					setObjectSubAnimation(wall1.id, "" .. 3)
				else
					setObjectSubAnimation(wall0.id, "" .. 3)
				end
			else
				setObjectSubAnimation(wall0.id, "" .. 3)
				setObjectSubAnimation(wall1.id, "" .. 3)
			end
		elseif (timer_time > (s*2)) then
			if (golems_beaten == 1) then
				if (fought_id == golem0.id) then
					setObjectSubAnimation(wall1.id, "" .. 2)
				else
					setObjectSubAnimation(wall0.id, "" .. 2)
				end
			else
				setObjectSubAnimation(wall0.id, "" .. 2)
				setObjectSubAnimation(wall1.id, "" .. 2)
			end
		elseif (timer_time > (s)) then
			if (golems_beaten == 1) then
				if (fought_id == golem0.id) then
					setObjectSubAnimation(wall1.id, "" .. 1)
				else
					setObjectSubAnimation(wall0.id, "" .. 1)
				end
			else
				setObjectSubAnimation(wall0.id, "" .. 1)
				setObjectSubAnimation(wall1.id, "" .. 1)
			end
		end


		local go = true
		if (px == 0) then
			px = px + 1
		elseif (py == 0) then
			py = py + 1
		elseif (px == 28) then
			px = px - 1
		elseif (py == 33) then
			py = py - 1
		else
			go = false
		end
		if (go) then
			doDialogue("Guard: Don't go! Take care of the Golems first!\n", false, false, true)
			setObjectPosition(0, px, py)
			astar_stop();
		end

		if (in_doorway(px, py)) then
			setObjectPosition(0, px, py+1)
			doDialogue("Guard: No time for that! The golems are breaking through our walls!\n", false, false, true)
		end
	end

	if (getMilestone(MS_KILLED_GOLEMS)) then
		if (px == 0 or py == 0 or px == 28 or py == 33) then
			doMap("flowey")
		end

		if (inn:update()) then
			change_areas("inn0", 10, 14, DIRECTION_NORTH)
		elseif (lib:update()) then
			change_areas("lib", 7, 10, DIRECTION_NORTH)
		elseif (mon:update()) then
			change_areas("monestary", 6, 14, DIRECTION_NORTH)
		elseif (barr:update()) then
			change_areas("flowey_barracks", 6, 14, DIRECTION_NORTH)
		elseif (shop:update()) then
			change_areas("flowey_shop", 11, 13, DIRECTION_NORTH)
		elseif (tav:update()) then
			change_areas("flowey_tavern", 6, 11, DIRECTION_NORTH)
		end
	end
end

function activate(activator, activated)
	if (getMilestone(MS_KILLED_GOLEMS)) then
		if (activated == guard0.id) then
			if (getMilestone(MS_BEAT_TODE)) then
				doDialogue("Guard: Whoa! You guys are amazing!\n")
			elseif (getMilestone(MS_BEACH_BATTLE_DONE)) then
				doDialogue("Guard: Some folks have returned to their home, but most are still afraid...\n")
			else
				doDialogue("Guard: Some of the people in the surrounding area have moved into the inn. It's dangerous out there!\n")
			end
		elseif (activated == guard1.id) then
			doDialogue("Guard: You're my hero!\n")
		elseif (activated == guard2.id) then
			if (not getMilestone(MS_FLOWEY_WALLS_FIXED)) then
				doDialogue("Guard: We'll get these walls fixed right away!\n")
			else
				doDialogue("Guard: The new walls should hold, as long as no more golems come by...\n")
			end
		elseif (activated == guard3.id) then
			if (not getMilestone(MS_FLOWEY_WALLS_FIXED)) then
				doDialogue("Guard: We'll get these walls fixed right away!\n")
			else
				doDialogue("Guard: Whew, I'm beat from fixing these walls.\n")
			end
		end
	else
		if (activated == golem0.id) then
			fought_id = golem0.id
			startBattle("1Golem")
			started_battle = true
			if (golems_beaten > 0) then
				setTimer(0, false)
			end
		elseif (activated == golem1.id) then
			fought_id = golem1.id
			startBattle("1Golem")
			started_battle = true
			if (golems_beaten > 0) then
				setTimer(0, false)
			end
		elseif (activated == guard0.id or activated == guard1.id or activated == guard2.id or activated == guard3.id) then
			doDialogue("Guard: Better hurry!\n", true, false, true)
		end
	end
end

function collide(id1, id2)
	if ((not (gate == nil)) and ((id1 == 0 and id2 == gate.id) or (id1 == gate.id and id2 == 0))) then
		remove_gate()
		gate_open_x, gate_open_y = getObjectPosition(0)
		can_close_gate = false
	end
end

