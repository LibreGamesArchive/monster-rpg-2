music = "volcano.ogg"

enemies = {}
enemies[0] = "1Efreet"
enemies[1] = "1FireAnt"
enemies[2] = "2Lavas2Vultures"
enemies[3] = "1BigBlue"
enemies[4] = "1Kromagma1Flamer1Zombie"
enemies[5] = "4Zombies"
enemies[6] = "1Flamer2Magmers"
enemies[7] = "2Flamers"
enemies[8] = "2Lavas"
enemies[9] = "2Magmods"

exceptions = {
	{ 16, 17 },
	{ 55, 15 },
	{ 61, 39 },
	{ 34, 38 },
	{ 36, 71 },
	{ 27, 40 },
	{ 28, 40 },
	{ 29, 40 },
	{ 30, 40 },
	{ 31, 40 },
	{ 32, 40 },
	{ 33, 40 },
	{ 34, 40 },
	{ 35, 40 },
	{ 36, 40 },
	{ 37, 40 },
	{ 38, 40 },
	{ 39, 40 },
	{ 40, 40 },
	{ 41, 40 },
	{ 42, 40 },
	{ 43, 40 },
	{ 44, 40 },
	{ 45, 40 },
	{ 46, 40 },
	{ 47, 40 },
	{ 48, 40 },
	{ 49, 40 },
}

bp = {
	{ 36, 59 },
	{ 37, 58 },
	{ 36, 57 },
	{ 39, 57 },
	{ 39, 56 },
	{ 37, 56 },
	{ 36, 55 },
	{ 38, 54 },
	{ 36, 53 },
	{ 34, 52 },
	{ 36, 52 },
	{ 34, 51 }, { 35, 50 }, { 36, 51 },
	{ 34, 49 }, { 35, 48 }, { 36, 49 },
	{ 34, 47 }, { 35, 46 }, { 36, 47 },
	{ 34, 45 }, { 35, 44 }, { 36, 45 },
	{ 34, 43 }, { 35, 42 }, { 36, 43 },
	{ 35, 41 }, { 36, 40 }, { 37, 41 },
	{ 35, 39 }, { 37, 39 },
}

local bridge_x1 = 27
local bridge_y1 = 39
local bridge_x2 = 49
local bridge_y2 = 40

local hide_x1 = 27
local hide_y1 = 40 
local hide_x2 = 49
local hide_y2 = 40

local tinting = false
local tint_count = 0
local reverse_tinting = false
local reverse_tint_count = 0

function raise_bridge()
	if (not getMilestone(MS_VOLCANO_BRIDGE_LOWERED)) then
		return
	end

	setMilestone(MS_VOLCANO_BRIDGE_LOWERED, false)

	local x, y, layer

	for layer=1,1,-1 do
		for y=bridge_y1,bridge_y2 do
			for x=bridge_x1,bridge_x2 do
				local val
				val = getTileLayer(x, y, layer)
				if (not (val == -1)) then
					setTileLayer(x, y, layer, -1)
					setTileLayer(x, y, layer+1, val)
				end
			end
		end
	end

	local i

	-- make blocks solid
	for i=1,#blocks do
		setObjectSolid(blocks[i].id, true)
	end

	-- unhide blocks under bridge
	for i=1,#blocks do
		local x, y = getObjectPosition(blocks[i].id)
		if (x >= hide_x1 and x <= hide_x2 and y >= hide_y1 and y <= hide_y2) then
			setObjectHidden(blocks[i].id, false)
		end
	end

	-- make bridge bounds unsolid
	for x=hide_x1,hide_x2 do
		setTileSolid(x, hide_y1-1, false)
		setTileSolid(x, hide_y1+1, false)
	end

	-- make ends solid
	setTileSolid(26, 40, true)
	setTileSolid(50, 40, true)
end

function lower_bridge(force)
	if ((not (force == true)) and getMilestone(MS_VOLCANO_BRIDGE_LOWERED)) then
		return
	end

	setMilestone(MS_VOLCANO_BRIDGE_LOWERED, true)

	local x, y, layer

	for layer=2,2 do
		for y=bridge_y1,bridge_y2 do
			for x=bridge_x1,bridge_x2 do
				local val
				val = getTileLayer(x, y, layer)
				if (not (val == -1)) then
					setTileLayer(x, y, layer, -1)
					setTileLayer(x, y, layer-1, val)
				end
			end
		end
	end

	-- make blocks not solid
	for i=1,#blocks do
		setObjectSolid(blocks[i].id, false)
	end
	
	-- hide blocks under bridge
	for i=1,#blocks do
		local x, y = getObjectPosition(blocks[i].id)
		if (x >= hide_x1 and x <= hide_x2 and y >= hide_y1 and y <= hide_y2) then
			setObjectHidden(blocks[i].id, true)
		end
	end

	-- make bridge bounds solid
	for x=hide_x1,hide_x2 do
		setTileSolid(x, hide_y1-1, true)
		setTileSolid(x, hide_y1+1, true)
	end

	-- make ends unsolid
	setTileSolid(26, 40, false)
	setTileSolid(50, 40, false)
end

function start()
	in1 = Portal:new{x=34, y=38}
	in2 = Portal:new{x=61, y=39}
	in3 = Portal:new{x=16, y=17}
	in4 = Portal:new{x=55, y=15}
	exit = Portal:new{x=36, y=71}

	blocks = {}
	for i=1,#bp do
		-- person, right....
		blocks[i] = Object:new{x=bp[i][1], y=bp[i][2], person=true, anim_set="rock", move_type=MOVE_PUSHED, pushed=false, rest=100}
	end
	
	lower_trigger = Object:new{x=14, y=38}
	raise_trigger = Object:new{x=14, y=41}
	setSpecialWalkable(lower_trigger.id, true)
	setSpecialWalkable(raise_trigger.id, true)

	if (getMilestone(MS_VOLCANO_BRIDGE_LOWERED)) then
		setObjectSolid(lower_trigger.id, false)
		lower_bridge(true)
	else
		setObjectSolid(raise_trigger.id, false)
	end
	
	chest1 = Chest:new{x=5, y=53, anim_set="blue_chest", milestone=MS_VOLCANO_CHEST_0, index=ITEM_ONYX_ARMOR}
	chest2 = Chest:new{x=6, y=53, anim_set="blue_chest", milestone=MS_VOLCANO_CHEST_1, index=ITEM_CURE3}

	if (not getMilestone(MS_BEAT_GIRL_DRAGON)) then
		girl = Object:new{x=34, y=11, anim_set="Girl"}
	end
end

function stop()
end

function update(step)
	if (battle_started) then
		if (battleWon) then
			slot = findUsedInventorySlot(ITEM_STAFF)
			setInventory(slot, -1, 0)
			stopObject(0)
			scriptifyPlayer()
			removeObject(girl.id)
			tint(64, 128, 255, 0)
			tinting = true
			battle_started = false
		end
	end

	if (in1:update()) then
		change_areas("volc_in-1", 20, 29, DIRECTION_NORTH)
	elseif (in2:update()) then
		change_areas("volc_in-2", 73, 29, DIRECTION_NORTH)
	elseif (exit:update()) then
		doMap("volcano")
	elseif (in3:update()) then
		change_areas("volc_in-4", 6, 29, DIRECTION_NORTH)
	elseif (in4:update()) then
		change_areas("volc_save", 7, 9, DIRECTION_NORTH)
	end

	for i=1,#blocks do
		blocks[i]:move(step)
	end

	check_battle(35, enemies, exceptions)

	if (tinting) then
		tint_count = tint_count + step
		if (tint_count > 2000) then
			tinting = false
			dpad_off()
			prepareForScreenGrab1()
			drawArea()
			dpad_on()
			drawBufferToScreen()
			prepareForScreenGrab2()
			fadeOut(0, 0, 0)
			doVolcanoEndScene()
			updateArea()
			dpad_off()
			prepareForScreenGrab1()
			drawArea()
			dpad_on()
			drawBufferToScreen()
			prepareForScreenGrab2()
			fadeIn(0, 0, 0)
			reverse_tint(64, 128, 255, 0)
			reverse_tinting = true
		end
	end

	if (reverse_tinting) then
		reverse_tint_count = reverse_tint_count + step
		if (reverse_tint_count > 2000) then
			reverse_tinting = false
			setObjectDirection(0, DIRECTION_SOUTH)
			updateArea()
			doDialogue("Eny: Wow... that was... something. But we can't forget about it.\nEny: We need a way to get to that staff...\n", true)
			setMilestone(MS_BEAT_GIRL_DRAGON, true)
			descriptifyPlayer()
		end
	end
end

function activate(activator, activated)
	if (activated == chest1.id) then
		chest1:activate()
	elseif (activated == chest2.id) then
		chest2:activate()
	elseif ((not (girl == nil)) and activated == girl.id) then
		doDialogue("Girl: Hand over the staff and I'll let you go free...\nEny: Never!\nGirl: Then let's fight for it!\n", true)
		startBattle("GirlDragon", true, false)
		battle_started = true
	end
end

function collide(id1, id2)
	if (id1 == 0 or id2 == 0) then
		for i=1,#blocks do
			if (blocks[i].pushed == false and (id1 == blocks[i].id or id2 == blocks[i].id)) then
				loadPlayDestroy("push.ogg")
				px, py = getObjectPosition(0)
				bx, by = getObjectPosition(blocks[i].id)
				blocks[i].pushed = true
				blocks[i].pushl = false
				blocks[i].pushr = false
				blocks[i].pushu = false
				blocks[i].pushd = false
				if (bx < px) then
					blocks[i].pushl = true
				elseif (bx > px) then
					blocks[i].pushr = true
				elseif (by < py) then
					blocks[i].pushu = true
				elseif (by > py) then
					blocks[i].pushd = true
				end
			end
		end
	end

	if (not getMilestone(MS_VOLCANO_BRIDGE_LOWERED)) then
		if (id1 == lower_trigger.id or id2 == lower_trigger.id) then
			lower_bridge()
			setObjectSolid(lower_trigger.id, false)
			setObjectSolid(raise_trigger.id, true)
		end
	else
		if (id1 == raise_trigger.id or id2 == raise_trigger.id) then
			raise_bridge()
			setObjectSolid(lower_trigger.id, true)
			setObjectSolid(raise_trigger.id, false)
		end
	end
end

