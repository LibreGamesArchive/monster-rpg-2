music = "moon2.ogg"

enemies = {}
enemies[0] = "2Relics"
enemies[1] = "3Devils"
enemies[2] = "4Shadows"
enemies[3] = "1Mummy2Shadows"
enemies[4] = "4Seekers"
enemies[5] = "2Evils"
enemies[6] = "2Awoken1Relic"
enemies[7] = "1DarkMan"

exceptions = {
	{ 71, 49 },
	{ 10, 8 },
}

local scene_started = false
local scene2 = false

function start()
	if (not getMilestone(MS_BEAT_TODE)) then
		tode = Object:new{x=10, y=10, anim_set="uberboss"}
	end

	portal = Object:new{x=9, y=9, anim_set="Portal", solid=false}
	setObjectLow(portal.id, true)

	chest1 = Chest:new{x=55, y=38, anim_set="blue_chest", milestone=MS_DARKSIDE_CHEST_1, index=ITEM_MEATBALLS}
	chest2 = Chest:new{x=65, y=18, anim_set="blue_chest", milestone=MS_DARKSIDE_CHEST_2, index=ITEM_HOLY_WATER, quantity=2}
	chest3 = Chest:new{x=48, y=7, anim_set="blue_chest", milestone=MS_DARKSIDE_CHEST_3, index=ITEM_ELIXIR, quantity=2}
	chest4 = Chest:new{x=48, y=18, anim_set="blue_chest", milestone=MS_DARKSIDE_CHEST_4, index=ITEM_CURE3, quantity=2}

	out = Portal:new{x=71, y=49}
end

function stop()
end

function done()
	if (battleWon) then
		removeObject(tode.id)
		setMilestone(MS_BEAT_TODE, true)
		doDialogue("Eny: We made it!...\n")
		scene_started = false
	end
	descriptifyPlayer()
end

function into()
	setObjectDirection(0, DIRECTION_SOUTH)
	doDialogue("Eny: Look everyone!...\n", true)
	prepareForScreenGrab1()
	drawArea()
	drawBufferToScreen()
	prepareForScreenGrab2()
	fadeOut(0, 0, 0)
	into_the_sun()
	updateArea()
	prepareForScreenGrab1()
	drawArea()
	drawBufferToScreen()
	prepareForScreenGrab2()
	fadeIn(0, 0, 0)
	doDialogue("Eny: We've done it! The staff is destroyed!\nLet's get back home now.\n", true)
	setMilestone(MS_SUN_SCENE, true)
end

function update(step)
	if (out:update()) then
		doMap("darkside", "map2")
	end

	if (scene_started) then
		eny:update(step)
	end

	local px, py = getObjectPosition(0)
	if ((not scene_started) and (not getMilestone(MS_BEAT_TODE)) and (px >= 3 and px <= 13 and py == 17)) then
		stopObject(0)
		scriptifyPlayer()
		eny = Object:new{id=0, x=px, y=py, person=true, direction=DIRECTION_NORTH}
		eny.scripted_events = {
			{ event_type=EVENT_WALK, dest_x=px, dest_y=12 },
			{ event_type=EVENT_WALK, dest_x=10, dest_y=12 },
			{ event_type=EVENT_LOOK, direction=DIRECTION_NORTH },
			{ event_type=EVENT_SPEAK, text="Eny: Look everyone! It's the Portal!\nTode: Noone shall pass.\nEny: In your dreams!\n", top=false },
			{ event_type=EVENT_WAIT_FOR_SPEECH },
			{ event_type=EVENT_BATTLE, id="1Tode", can_run=false },
			{ event_type=EVENT_WAIT_FOR_BATTLE },
			{ event_type=EVENT_CUSTOM, callback=done },
			
		}
		scene_started = true
	end

	if (not scene_started) then
		check_battle(30, enemies, exceptions)
	end

	if (not scene2 and px == 10 and py == 8) then
		stopObject(0)
		scene2 = true
		if (not getMilestone(MS_SUN_SCENE)) then
			into()
		end
		loadPlayDestroy("Portal.ogg")
		setMilestone(MS_ON_MOON, false)
		change_areas("portal", 9, 8, DIRECTION_SOUTH)
	end
end

function activate(activator, activated)
	if (activated == chest1.id) then
		chest1:activate()
	elseif (activated == chest2.id) then
		chest2:activate()
	elseif (activated == chest3.id) then
		chest3:activate()
	elseif (activated == chest4.id) then
		chest4:activate()
	end
end

function collide(id1, id2)
end

