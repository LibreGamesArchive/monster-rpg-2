music = "fortress.ogg"

local tig_scene_active = false
local rider_removed = false

local trap_x = 46
local trap_y = 4
local trap_w = 13
local trap_h = 5

safe_blocks = {
	{ 50, 5 },
	{ 51, 5 },
	{ 51, 6 },
	{ 51, 7 },
	{ 51, 8 },
	{ 52, 5 },
	{ 52, 8 },
	{ 53, 8 },
	{ 53, 7 },
	{ 53, 6 },
	{ 54, 6 },
	{ 54, 5 },
}

WATER = getAnimationNum(1450)
LAND = getAnimationNum(1354)

function fall(fx, fy)
	stopObject(0)
	astar_stop()
	loadPlayDestroy("splash.ogg")
	setObjectHidden(0, true)
	setTileLayer(fx, fy, 0, WATER)
	dpad_off()
	drawArea()
	dpad_on()
	flip()
	rest(2)
	fadeOut()
	setObjectHidden(0, false)
	setObjectPosition(0, 52, 8)
	local x
	local y
	for y=1,trap_h do
		for x=1,trap_w do
			setTileLayer(x+trap_x-1, y+trap_y-1, 0, LAND)
		end
	end
	local num_dead = 0
	local num_players = 0

	-- take hp
	for i=1,4 do
		local now = getPlayerHP(i-1)
		if (now == 1) then
			setPlayerHP(i-1, 0)
			num_dead = num_dead + 1
			num_players = num_players + 1
		elseif (now > 1) then
			setPlayerHP(i-1, 1)
			num_players = num_players + 1
		end
	end

	if (not (num_dead == num_players)) then
		setObjectDirection(0, DIRECTION_NORTH)
		clearBuffer()
		updateArea()
		dpad_off()
		drawArea()
		dpad_on()

		fadeIn()
		doDialogue("Eny: Ow...\n")
	else
		anotherDoDialogue("You drown in the cold sea...\n", true)
	end
end

local pool_x = 31
local pool_y = 23
local pool_w = 13
local pool_h = 8
local started = false

function cb1()
	setTileSolid(37, 23, false)
	for x=1,pool_w do
		setTileLayer(x+pool_x-1, pool_y, 2, -1)
	end
end

function cb2()
	for x=1,pool_w do
		setTileLayer(x+pool_x-1, pool_y+1, 2, -1)
	end
end

function cb3()
	for x=1,pool_w do
		setTileLayer(x+pool_x-1, pool_y+2, 2, -1)
	end
end

function cb4()
	for y=4,pool_h do
		for x=1,pool_w do
			setTileLayer(x+pool_x-1, pool_y+y-1, 2, -1)
		end
	end

	descriptifyPlayer()
	setMilestone(MS_DRAINED_POOL, true)
end

function total_drain()
	setTileSolid(37, 23, false)
	for y=1,pool_h do
		for x=1,pool_w do
			setTileLayer(x+pool_x-1, pool_y+y-1, 2, -1)
		end
	end
end

function add_scope_chest()
	if (getMilestone(MS_DROPPED_MEDALLION)) then
		scope_chest = Chest:new{x=22, y=7, anim_set="chest", milestone=MS_GOT_LOOKING_SCOPE, index=ITEM_LOOKING_SCOPE}
		setObjectSubAnimation(medallion_box.id, "on")	
	end
end


exceptions = {
	{ 9, 19 },
	{ 14, 17 },
	{ 37, 6 },
	{ 37, 7 },
	{ 37, 8 },
	{ 60, 19 },
	{ 65, 17 },
	{ 60, 42 },
	{ 65, 40 },
	{ 9, 42 },
	{ 14, 40 },
}

pool_down_solids = {
	{ 30, 26, 30, 31 },
	{ 30, 31, 44, 31 },
	{ 44, 31, 44, 26 },
}

pool_up_solids = {
	{ 31, 26, 31, 30 },
	{ 31, 30, 43, 30 },
	{ 43, 30, 43, 26 },
}

local is_up = true


function removeEny()
	pushPlayers()
	clearPlayers()
	addPlayer("Rider")
	addPlayer("Faelon")
	addPlayer("Mel")
end


function removeOthers()
	clearPlayers()
	addPlayer("Eny")
end

function restore()
	popPlayers()
	reviveAllPlayers()
	setObjectSubAnimation(rider.id, "stand_n")
	setObjectSubAnimation(mel.id, "stand_n")
	setObjectSubAnimation(faelon.id, "stand_n")
	setObjectAnimationSetPrefix(tig.id, "")
	setObjectSubAnimation(tig.id, "stand_s")
	staff = Object:new{x=36, y=5, anim_set="staff_broken"}
	setObjectSolid(staff.id, false)
end

function remove_staff()
	removeObject(staff.id)
end

function remove_rider()
	removeObject(rider.id)
	rider_removed = true
end


function end_scene()
	setObjectSolid(0, true)
	removePartyMember("Rider")
	addPartyMember("Tiggy")
	removeObject(tig.id)
	removeObject(faelon.id)
	removeObject(mel.id)
	descriptifyPlayer()
	setMilestone(MS_BEAT_TIGGY, true)
	--local slot = findUsedInventorySlot(ITEM_STAFF);
	--setInventory(slot, -1, 0);
	tig_scene_active = false
end


function start()
	samp = loadSample("drain.ogg")

	sub = Object:new{x=34, y=51, anim_set="sub"}
	setObjectLow(sub.id, true)
	gunnar = Object:new{x=36, y=48, anim_set="Gunnar", person=true}
	setObjectDirection(gunnar.id, DIRECTION_NORTH);

	down1 = Portal:new{x=60, y=19}
	down2 = Portal:new{x=60, y=42}
	down3 = Portal:new{x=9, y=42}
	down4 = Portal:new{x=9, y=19}

	up1 = Portal:new{x=65, y=17}
	up2 = Portal:new{x=65, y=40}
	up3 = Portal:new{x=14, y=40}
	up4 = Portal:new{x=14, y=17}

	alter1 = Object:new{x=44, y=22, anim_set="alter"}
	alter2 = Object:new{x=44, y=31, anim_set="alter"}
	alter3 = Object:new{x=30, y=31, anim_set="alter"}
	alter4 = Object:new{x=30, y=22, anim_set="alter"}

	if (getMilestone(MS_LIT_ALTER1)) then
		setObjectSubAnimation(alter1.id, "on")
	end
	if (getMilestone(MS_LIT_ALTER2)) then
		setObjectSubAnimation(alter2.id, "on")
	end
	if (getMilestone(MS_LIT_ALTER3)) then
		setObjectSubAnimation(alter3.id, "on")
	end
	if (getMilestone(MS_LIT_ALTER4)) then
		setObjectSubAnimation(alter4.id, "on")
	end

	if (getMilestone(MS_DRAINED_POOL)) then
		total_drain()
	end


	if (not getMilestone(MS_GOT_KEY2)) then
		key = Chest:new{x=37, y=27, anim_set="key", milestone=MS_GOT_KEY2, index=ITEM_KEY2}
	end

	door1 = Door:new{x=21, y=12, width=3, anim_set="fortress_door"}
	door2 = Door:new{x=36, y=12, width=3, anim_set="fortress_door"}
	door3 = Door:new{x=51, y=12, width=3, anim_set="fortress_door"}
	door4 = Door:new{x=37, y=45, width=3, anim_set="fortress_door"}

	local px, py = getObjectPosition(0)
	if ((px == 22 and py == 12) or (px == 22 and py == 11)) then
		removeObject(door1.id)
		door1 = nil
	end
	if ((px == 37 and py == 12) or (px == 37 and py == 11)) then
		removeObject(door2.id)
		door2 = nil
	end
	if ((px == 52 and py == 12) or (px == 52 and py == 11)) then
		removeObject(door3.id)
		door3 = nil
	end
	if ((px == 38 and py == 45) or (px == 38 and py == 44)) then
		removeObject(door4.id)
		door4 = nil
	end

	medallion_box = Object:new{x=22, y=4, anim_set="medallion_box"}
	
	add_scope_chest()

	trap_chest1 = Chest:new{x=50, y=4, anim_set="chest", milestone=MS_FORT_TRAPPED_CHEST1, index=ITEM_CURE2}
	trap_chest2 = Chest:new{x=52, y=4, anim_set="chest", milestone=MS_FORT_TRAPPED_CHEST2, index=ITEM_MYSTIC_BLADE}
	trap_chest3 = Chest:new{x=54, y=4, anim_set="chest", milestone=MS_FORT_TRAPPED_CHEST3, index=ITEM_ELIXIR}


	down_trigger = Object:new{x=37, y=25}
	up_trigger = Object:new{x=37, y=24}
	setObjectSolid(up_trigger.id, false)
	setSpecialWalkable(down_trigger.id, true)
	setSpecialWalkable(up_trigger.id, true)

	if (not getMilestone(MS_BEAT_TIGGY)) then
		tig = Object:new{x=37, y=4, anim_set="Tiggy", person=true}
		setObjectDirection(tig.id, DIRECTION_SOUTH)
		setObjectSolid(tig.id, false)
		setObjectAnimationSetPrefix(tig.id, "staff_")
	end
end

function stop()
	destroySample(samp)
end

function update(step)
	if (not getMilestone(MS_SUB_SCENE_DONE)) then
		setMilestone(MS_SUB_SCENE_DONE, true)
		descriptifyPlayer()
	end

	if (down1:update()) then
		change_areas("fort1--1", 5, 7, DIRECTION_NORTH)
	elseif (down2:update()) then
		change_areas("fort2--1", 5, 7, DIRECTION_NORTH)
	elseif (down3:update()) then
		change_areas("fort3--1", 5, 7, DIRECTION_NORTH)
	elseif (down4:update()) then
		change_areas("fort4--1", 5, 7, DIRECTION_NORTH)
	elseif (up1:update()) then
		change_areas("fort1-2", 6, 4, DIRECTION_SOUTH)
	elseif (up2:update()) then
		change_areas("fort2-2", 6, 4, DIRECTION_SOUTH)
	elseif (up3:update()) then
		change_areas("fort3-2", 6, 4, DIRECTION_SOUTH)
	elseif (up4:update()) then
		change_areas("fort4-2", 6, 4, DIRECTION_SOUTH)
	end

	if (started and not getMilestone(MS_DRAINED_POOL)) then
		tmp:update(step)
	end

	local px, py = getObjectPosition(0)
	if (px >= trap_x and px < trap_x+trap_w and
			py >= trap_y and py < trap_y+trap_h) then
		local safe = false
		for i=1,#safe_blocks do
			if ((px == safe_blocks[i][1]) and (py == safe_blocks[i][2])) then
				safe = true
				break
			end
		end
		if (not safe) then
			fall(px, py)
		end
	end
		
	local px, py = getObjectPosition(0)

	if (not getMilestone(MS_BEAT_TIGGY) and not tig_scene_active and px == 37 and py == 9) then
		tig_scene_active = true
		reviveAllPlayers()
		stopObject(0)
		scriptifyPlayer()
		setObjectSolid(0, false)
		eny = Object:new{id=0, x=px, y=py, person=true}
		setObjectDirection(0, DIRECTION_NORTH)
		rider = Object:new{x=px-1, y=py, anim_set="Rider", person=true}
		faelon = Object:new{x=px, y=py-1, anim_set="Faelon", person=true}
		mel = Object:new{x=px+1, y=py, anim_set="fleer", person=true}
		setObjectSolid(rider.id, false)
		setObjectSolid(faelon.id, false)
		setObjectSolid(mel.id, false)
		eny.scripted_events = {
			{ event_type=EVENT_REST, delay=1200 },
			{ event_type=EVENT_WALK, dest_x=px, dest_y=py-2 },
			{ event_type=EVENT_SPEAK, text="Eny: Tiggy! I'm so glad you're alright!\nTig: Do I know you?\nEny: Tig, it's me, your friend Eny...\nTig: You don't look like a friend of mine.\nFaelon: Enough of this talk, he is the root of our problems...\nRider: He must be defeated!\nEny: No! Don't hurt him!\nTig: Hurt me?! Ahahaha... you must be playing.\n"},
			{ event_type=EVENT_WAIT_FOR_SPEECH },
			{ event_type=EVENT_CUSTOM, callback=removeEny },
			{ event_type=EVENT_BATTLE, id="2Statues", must_win=false },
			{ event_type=EVENT_WAIT_FOR_BATTLE },
			{ event_type=EVENT_SPEAK, bottom=true, text="Tig: You see... You are no match for me!\nEny: Please Tiggy, listen to me...\n"},
			{ event_type=EVENT_WAIT_FOR_SPEECH },
			{ event_type=EVENT_WALK, dest_x=px, dest_y=py-3},
			{ event_type=EVENT_SPEAK, bottom=true, text="Tig: You're wasting my time!\n"},
			{ event_type=EVENT_WAIT_FOR_SPEECH },
			{ event_type=EVENT_CUSTOM, callback=removeOthers },
			{ event_type=EVENT_BATTLE, id="tig_end" },
			{ event_type=EVENT_WAIT_FOR_BATTLE },
			{ event_type=EVENT_CUSTOM, callback=restore },
			{ event_type=EVENT_REST, delay=2000 },
			{ event_type=EVENT_SYNC, who=rider, number=12 },
			{ event_type=EVENT_SPEAK, text="Tiggy: So where do we go from here?\nEny: The abandoned Keep north of Seaside... \\ They're building an army!\nMel: Let's go, we don't have time to waste!\nEny: Hey, where's the staff...\nEny: THE STAFF!!! \\ And where is Rider?\nTiggy: I saw him leave with the staff, I thought you knew?\nEny: Oh no, we have to find him! I think he will be at the Keep.\nTiggy: Then let's go!\n" },
			{ event_type=EVENT_WAIT_FOR_SPEECH },
			{ event_type=EVENT_REST, delay=1500 },
			{ event_type=EVENT_CUSTOM, callback=end_scene },
		}
		rider.scripted_events = {
			{ event_type=EVENT_WALK, dest_x=px-1, dest_y=py-3},
			{ event_type=EVENT_SYNC, who=eny, number=7 },
			{ event_type=EVENT_GESTURE, name="unconscious" },
			{ event_type=EVENT_SYNC, who=eny, number=17 },
			{ event_type=EVENT_WALK, dest_x=36, dest_y=5 },
			{ event_type=EVENT_CUSTOM, callback=remove_staff },
			{ event_type=EVENT_SYNC, who=tig, number=6 },
			{ event_type=EVENT_WALK, dest_x=33, dest_y=5 },
			{ event_type=EVENT_WALK, dest_x=33, dest_y=8 },
			{ event_type=EVENT_WALK, dest_x=37, dest_y=8 },
			{ event_type=EVENT_WALK, dest_x=37, dest_y=9 },
			{ event_type=EVENT_CUSTOM, callback=remove_rider },
		}
		faelon.scripted_events = {
			{ event_type=EVENT_WALK, dest_x=px, dest_y=py-4},
			{ event_type=EVENT_SYNC, who=eny, number=7 },
			{ event_type=EVENT_CUSTOM, callback=faelon_fall },
			{ event_type=EVENT_SYNC, who=eny, number=20 },
			{ event_type=EVENT_WALK, dest_x=37, dest_y=6 },
			{ event_type=EVENT_SYNC, who=eny, number=22 },
		}
		mel.scripted_events = {
			{ event_type=EVENT_WALK, dest_x=px+1, dest_y=py-3},
			{ event_type=EVENT_SYNC, who=eny, number=7 },
			{ event_type=EVENT_GESTURE, name="unconscious" },
			{ event_type=EVENT_SYNC, who=eny, number=20 },
			{ event_type=EVENT_WALK, dest_x=37, dest_y=6 },
			{ event_type=EVENT_SYNC, who=eny, number=22 },
		}
		tig.scripted_events = {
			{ event_type=EVENT_SYNC, who=eny, number=17 },
			{ event_type=EVENT_GESTURE, name="blink" },
			{ event_type=EVENT_REST, delay=2000 },
			{ event_type=EVENT_GESTURE, name="stand_s" },
			{ event_type=EVENT_SPEAK, text="Tiggy: Eny? Who are these people? Where are we?...\nEny: Tiggy! That staff, it possessed you! You weren't yourself...\nFaelon: He darned near killed us all!\nEny: Faelon, it wasn't his fault! It's the staff...\nTiggy: I'm very sorry, I was not in control of myself.\n" },
			{ event_type=EVENT_WAIT_FOR_SPEECH },
			{ event_type=EVENT_SYNC, who=eny, number=20 },
			{ event_type=EVENT_WALK, dest_x=37, dest_y=6 },
			{ event_type=EVENT_SYNC, who=eny, number=22 },
		}
	end


	if (not tig_scene_active) then
		check_battle(25, fort_enemies, exceptions)
	end

	if (tig_scene_active) then
		eny:update(step)
		if (not rider_removed) then
			rider:update(step)
		end
		faelon:update(step)
		mel:update(step)
		tig:update(step)
	end
end

function activate(activator, activated)
	if (activated == gunnar.id) then
		local v = prompt("Board submarine?", "", 0, 1)
		if (v) then
			setMilestone(MS_DOCK_TO_FORT, false)
			setMilestone(MS_FORT_TO_DOCK, true)
			doMap("fortress")
		end
	elseif (activated == alter1.id and not getMilestone(MS_LIT_ALTER1)) then
		local alter = alter1
		local ms = MS_LIT_ALTER1
		local slot, quantity = findUsedInventorySlot(ITEM_FLINT)
		if (slot >= 0) then
			-- remove one flint
			if (quantity == 1) then
				setInventory(slot, -1, 0)
			else
				setInventory(slot, ITEM_FLINT, quantity-1)
			end
			-- light the alter
			loadPlayDestroy("ignite.ogg")
			setMilestone(ms, true)
			setObjectSubAnimation(alter.id, "on")
		elseif (not getMilestone(ms)) then
			doDialogue("We could light these if we could get a spark...\n")
		end
	elseif (activated == alter2.id and not getMilestone(MS_LIT_ALTER2)) then
		local alter = alter2
		local ms = MS_LIT_ALTER2
		local slot, quantity = findUsedInventorySlot(ITEM_FLINT)
		if (slot >= 0) then
			-- remove one flint
			if (quantity == 1) then
				setInventory(slot, -1, 0)
			else
				setInventory(slot, ITEM_FLINT, quantity-1)
			end
			-- light the alter
			loadPlayDestroy("ignite.ogg")
			setMilestone(ms, true)
			setObjectSubAnimation(alter.id, "on")
		elseif (not getMilestone(ms)) then
			doDialogue("We could light these if we could get a spark...\n")
		end
	elseif (activated == alter3.id and not getMilestone(MS_LIT_ALTER3)) then
		local alter = alter3
		local ms = MS_LIT_ALTER3
		local slot, quantity = findUsedInventorySlot(ITEM_FLINT)
		if (slot >= 0) then
			-- remove one flint
			if (quantity == 1) then
				setInventory(slot, -1, 0)
			else
				setInventory(slot, ITEM_FLINT, quantity-1)
			end
			-- light the alter
			loadPlayDestroy("ignite.ogg")
			setMilestone(ms, true)
			setObjectSubAnimation(alter.id, "on")
		elseif (not getMilestone(ms)) then
			doDialogue("We could light these if we could get a spark...\n")
		end
	elseif (activated == alter4.id and not getMilestone(MS_LIT_ALTER4)) then
		local alter = alter4
		local ms = MS_LIT_ALTER4
		local slot, quantity = findUsedInventorySlot(ITEM_FLINT)
		if (slot >= 0) then
			-- remove one flint
			if (quantity == 1) then
				setInventory(slot, -1, 0)
			else
				setInventory(slot, ITEM_FLINT, quantity-1)
			end
			-- light the alter
			loadPlayDestroy("ignite.ogg")
			setMilestone(ms, true)
			setObjectSubAnimation(alter.id, "on")
		elseif (not getMilestone(ms)) then
			doDialogue("We could light these if we could get a spark...\n")
		end
	elseif (activated == trap_chest1.id) then
		trap_chest1:activate()
	elseif (activated == trap_chest2.id) then
		trap_chest2:activate()
	elseif (activated == trap_chest3.id) then
		trap_chest3:activate()
	end

	if (not getMilestone(MS_DRAINED_POOL)) then
		if (
				getMilestone(MS_LIT_ALTER1) and
				getMilestone(MS_LIT_ALTER2) and
				getMilestone(MS_LIT_ALTER3) and
				getMilestone(MS_LIT_ALTER4)) then
			started = true
			-- start scene
			playSample(samp)
			scriptifyPlayer()
			px, py = getObjectPosition(0)
			tmp = Object:new{id=0, x=px, y=py}
			tmp.scripted_events = {
				{ event_type=EVENT_REST, delay=2000 },
				{ event_type=EVENT_CUSTOM, callback=cb1 },
				{ event_type=EVENT_REST, delay=2000 },
				{ event_type=EVENT_CUSTOM, callback=cb2 },
				{ event_type=EVENT_REST, delay=2000 },
				{ event_type=EVENT_CUSTOM, callback=cb3 },
				{ event_type=EVENT_REST, delay=2000 },
				{ event_type=EVENT_CUSTOM, callback=cb4 },
			}
		end
	end

	if (not getMilestone(MS_GOT_KEY2)) then
		if (activated == key.id) then
			key:activate()
			removeObject(key.id)
		end
	end

	if (not getMilestone(MS_DROPPED_MEDALLION)) then
		if (activated == medallion_box.id) then
			if (getMilestone(MS_GOT_MEDALLION)) then
				loadPlayDestroy("appear.ogg")
				setMilestone(MS_DROPPED_MEDALLION, true)
				local s, q = findUsedInventorySlot(ITEM_MEDALLION)
				setInventory(s, -1, 0)
				add_scope_chest()
			else
				doDialogue("Looks like a hole for a medallion...\n")
			end
		end
	else
		if (activated == scope_chest.id) then
			scope_chest:activate()
		end
	end
end


function _set(value, list)
	for i=1,#list do
		local x, y
		local x1, x2
		local y1, y2
		if (list[i][1] < list[i][3]) then
			x1 = list[i][1]
			x2 = list[i][3]
		else
			x2 = list[i][1]
			x1 = list[i][3]
		end
		if (list[i][2] < list[i][4]) then
			y1 = list[i][2]
			y2 = list[i][4]
		else
			y2 = list[i][2]
			y1 = list[i][4]
		end
		for y=y1,y2 do
			for x=x1,x2 do
				setTileSolid(x, y, value)
			end
		end
	end
end

function set_unsolid(list)
	_set(false, list)
end

function set_solid(list)
	_set(true, list)
end


function faelon_fall()
	local x, y
	x, y = getObjectPosition(faelon.id)
	setObjectPosition(faelon.id, x+1, y)
	setObjectSubAnimation(faelon.id, "unconscious")
end


function collide(id1, id2)
	if ((not (door1 == nil)) and (id1 == door1.id or id2 == door1.id)) then
		door1:open()
		removeObject(door1.id)
		door1 = nil
	elseif ((not (door2 == nil)) and (id1 == door2.id or id2 == door2.id)) then
		if (getMilestone(MS_GOT_KEY2)) then
			-- take key
			local slot, quantity = findUsedInventorySlot(ITEM_KEY2)
			if (slot >= 0) then
				setInventory(slot, -1, 0)
			end
			door2:open()
			removeObject(door2.id)
			door2 = nil
		else
			doDialogue("Locked...\n")
		end
	elseif ((not (door3 == nil)) and (id1 == door3.id or id2 == door3.id)) then
		door3:open()
		removeObject(door3.id)
		door3 = nil
	elseif ((not (door4 == nil)) and (id1 == door4.id or id2 == door4.id)) then
		door4:open()
		removeObject(door4.id)
		door4 = nil
	end

	if (is_up) then
		if (id1 == down_trigger.id or id2 == down_trigger.id) then
			is_up = false
			set_unsolid(pool_up_solids)
			set_solid(pool_down_solids)
			setObjectSolid(down_trigger.id, false)
			setObjectSolid(up_trigger.id, true)
		end
	else
		if (id1 == up_trigger.id or id2 == up_trigger.id) then
			is_up = true
			set_unsolid(pool_down_solids)
			set_solid(pool_up_solids)
			setObjectSolid(down_trigger.id, true)
			setObjectSolid(up_trigger.id, false)
		end
	end
end

