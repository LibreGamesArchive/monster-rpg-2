music = "keep.caf"

enemies = {}
enemies[0] = "2Mages1Halberd"
enemies[1] = "3Stabbers"
enemies[2] = "2Stabbers2Halberds"
enemies[3] = "1Mage2Stabbers"

exceptions = {
	{19,3},
	{19,12},
	{3,23},
	{34,23},
}


bp = {
	{ 4, 18 },
	{ 6, 18 },
	{ 9, 18 },
	{ 5, 19 },
	{ 8, 19 },
	{ 6, 20 },
	{ 7, 20 },
	{ 9, 20 },
	{ 5, 21 },
	{ 7, 21 },
	{ 9, 21 },
	{ 4, 22 },
	{ 5, 22 },
	{ 6, 22 },
	{ 8, 22 },
	{ 4, 23 },
}

function start()
	down = Portal:new{x=19, y=3}
	up1 = Portal:new{x=3, y=23}
	up2 = Portal:new{x=34, y=23}
	up_to_3 = Portal:new{x=19, y=12}

	sm_chest = Chest:new{x=9, y=4, anim_set="chest", milestone=MS_KEEP_2_CHEST_0, index=ITEM_CURE2, quantity=5}
	big_chest = Chest:new{x=27, y=9, anim_set="big_chest", milestone=MS_KEEP_2_CHEST_1, index=ITEM_KEEP_KEY_OUT}
	setObjectDimensions(big_chest.id, 32, 32)

	blocks = {}
	for i=1,#bp do
		-- person, right....
		blocks[i] = Object:new{x=bp[i][1], y=bp[i][2], person=true, anim_set="pushbox", move_type=MOVE_PUSHED, pushed=false, rest=100}
	end
end

function stop()
end

function update(step)
	check_battle(30, enemies, exceptions)

	if (down:update()) then
		change_areas("Keep_1", 19, 3, DIRECTION_SOUTH)
	elseif (up1:update()) then
		setMilestone(MS_GONE_TO_ARCHERY_TOWER, true)
		change_areas("Keep_outer", 6, 24, DIRECTION_SOUTH, false) -- don't transition in!
	elseif (up2:update()) then
		setMilestone(MS_GONE_TO_TROLL_TOWER, true)
		change_areas("Keep_outer", 31, 24, DIRECTION_NORTH)
	elseif (up_to_3:update()) then
		change_areas("Keep_3", 14, 12, DIRECTION_SOUTH)
	end

	for i=1,#blocks do
		blocks[i]:move(step)
	end
end

function activate_big_chest()
	big_chest:activate()
end

function activate(activator, activated)
	if (activated == sm_chest.id) then
		sm_chest:activate()
	elseif (activated == big_chest.id) then
		activate_big_chest()
	end
end

function activate_any(tx, ty)
	local px, py = getObjectPosition(0)
	if ((px == 27 and py == 10 and tx == 27 and ty == 8) or
		(px == 28 and py == 10 and tx == 28 and ty == 8)) then
		setObjectDirection(0, DIRECTION_NORTH)
		activate_big_chest()
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
end

