music = "12okt.ogg"

local rocket_launched = false
local scene_started = false
local rocket_count = 0

function start()
	gate = Portal:new{x=0, y=7}

	farmer = Object:new{x=8, y=10, anim_set="Farmer", person=true}
	setObjectDirection(farmer.id, DIRECTION_SOUTH)

	if (not getMilestone(MS_ROCKET_LAUNCHED)) then
		rocket = addRocket(9*TILE_SIZE, 8*TILE_SIZE, "rocket")
		black = Object:new{x=11, y=7, anim_set="black"}
		setObjectLow(black.id, true)
	end
end

function stop()
end

function update(step)
	if (gate:update()) then
		change_areas("farm", 32, 24, DIRECTION_WEST)
	end

	if (rocket_launched) then
		rocket_count = rocket_count + step
		if (rocket_count > 20000) then
			dpad_off()
			drawArea()
			dpad_on()
			fadeOut(0, 0, 0)
			clearBuffer(0xff, 0xd8, 0x00)
			rest(2)
			notify("We're going to land now...", "Fire your left and right", "thrusters to maneuver.")
			notify("The landing area is", "marked so you can see it!", "")
			dpad_off()
			fadeOut(0, 0, 0)
			dpad_on()
			rest(3)
			doLander()
			rocket_launched = false
			setObjectSolid(0, true)
			setObjectHidden(0, false)
			descriptifyPlayer()
			if (landerSuccess()) then
				change_areas("moon_landing", 8, 11, DIRECTION_SOUTH)
			else
				gameOver()
			end
		end
	end

	if (scene_started) then
		if (faelon.scripted_events) then
			faelon:update(step)
		elseif (eny.scripted_events) then
			eny:update(step)
		end
	end
end

function eny_go()
	faelon.scripted_events = nil
	removePartyMember("Faelon")
	local px, py = getObjectPosition(0)
	if (not (px > 8)) then
		px = 8
	end
	py = 10
	eny.scripted_events = {
		{ event_type=EVENT_WALK, dest_x=px, dest_y=py },
		{ event_type=EVENT_WALK, dest_x=11, dest_y=10 },
		{ event_type=EVENT_WALK, dest_x=11, dest_y=7 },
		{ event_type=EVENT_CUSTOM, callback=done },
	}
end

function done()
	setObjectHidden(0, true)
	loadPlayDestroy("rocket_launch.ogg")
	setObjectSubAnimation(rocket, "door_closed")
	removeObject(black.id)
	startRocket(rocket)
	scene_started = false
	rocket_launched = true
	setMilestone(MS_ROCKET_LAUNCHED, true)
end

function activate(activator, activated)
	if (activated == farmer.id) then
		local _d = getObjectDirection(farmer.id)
		setObjectDirection(farmer.id, player_dir(farmer))
		local slot = findUsedInventorySlot(ITEM_LOOKING_SCOPE)
		if (getMilestone(MS_ROCKET_LAUNCHED)) then
			doDialogue("Farmer: She flew! I can't believe she flew!\n", true)
		elseif (slot >= 0) then
			stopObject(0)
			scriptifyPlayer()
			doDialogue("Farmer: Hyaaa! I'm so happy somebody's gunna be ridin' in my spaceship! Hop in fellers! It's all set to go to the Moon!\n", true)
			local px, py = getObjectPosition(0)
			eny = Object:new{id=0, x=px, y=py}
			setObjectSolid(0, false)
			faelon = Object:new{x=px, y=py, anim_set="Faelon", person=true}
			setObjectSolid(faelon.id, false)
			setObjectDirection(0, DIRECTION_SOUTH)
			faelon.scripted_events = {
				{ event_type=EVENT_WALK, dest_x=8, dest_y=12 },
				{ event_type=EVENT_LOOK, direction=DIRECTION_NORTH },
				{ event_type=EVENT_SPEAK, text="Faelon: I must stay here and protect the Kingdom. I'm sorry that I cannot go with you.\nEny: Thank you Faelon, you have been a great companion.\nEny: The rest of us must go now.\n", top=true },
				{ event_type=EVENT_CUSTOM, callback=eny_go },
			}
			scene_started = true
		else
			doDialogue("Ya can't board the rocket without proper equipment! You need a Looking Scope at least!\n")
		end
		setObjectDirection(farmer.id, _d)
	end
end

function collide(id1, id2)
end

