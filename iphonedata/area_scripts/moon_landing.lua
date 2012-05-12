music = "landing.ogg"

local count = 0
local scene_started = false

function set_solids(state)
	setTileSolid(4, 6, state)
	setTileSolid(5, 6, state)
	setTileSolid(7, 6, state)
	setTileSolid(8, 6, state)
	setTileSolid(6, 5, state)
end

function start()
	samp = loadSample("rocket_launch.ogg")

	setMilestone(MS_ON_MOON, true)
	
	if (not getMilestone(MS_LANDER_LAUNCHED)) then
		lander = addRocket(4*TILE_SIZE, 7*TILE_SIZE, "lander");
		setObjectSolid(lander, true)
		set_solids(true)
	end

	out = Portal:new{x=10, y=14, width=2, height=1}
end

function stop()
	destroySample(samp)
end

function deposit()
	loadPlayDestroy("chest.ogg")
	setObjectSubAnimation(lander, "door_closed")
end

function launch()
	setObjectDirection(0, DIRECTION_NORTH)
	playSample(samp)
	startRocket(lander)
	set_solids(false)
end

function done()
	setMilestone(MS_LANDER_LAUNCHED, true)
	descriptifyPlayer()
	scene_started = false
end

function update(step)
	if (out:update()) then
		doMap("landing", "map2")
	end

	if (scene_started) then
		eny:update(step)
	end
	
	if (count == 1 and not getMilestone(MS_LANDER_LAUNCHED)) then
		local slot = findUsedInventorySlot(ITEM_STAFF)
		if (slot >= 0) then
			setInventory(slot, -1, 0)
			local px, py = getObjectPosition(0)
			eny = Object:new{id=0, x=px, y=py, person=true}
			stopObject(0)
			scriptifyPlayer()
			eny.scripted_events = {
				{ event_type=EVENT_WALK, dest_x=6, dest_y=13 },
				{ event_type=EVENT_WALK, dest_x=6, dest_y=6 },
				{ event_type=EVENT_CUSTOM, callback=deposit },
				{ event_type=EVENT_WALK, dest_x=6, dest_y=11 },
				{ event_type=EVENT_CUSTOM, callback=launch },
				{ event_type=EVENT_REST, delay=10000 },
				{ event_type=EVENT_SPEAK, text="Eny: Well there it goes! We'll have to wait and see if it gets there or not...\nEny: Now, let's find that Portal on the Dark Side!\n", top=true },
				{ event_type=EVENT_WAIT_FOR_SPEECH },
				{ event_type=EVENT_CUSTOM, callback=done },
			}
			scene_started = true
		end
	end

	count = count + 1
end

function activate(activator, activated)
end

function collide(id1, id2)
end

