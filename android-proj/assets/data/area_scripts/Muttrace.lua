music = "Muttrace.caf"

local stopped = false


local low_solids = {
	{ 24, 15 },
	{ 16, 15 },
}

local high_solids = {
	{ 19, 22 },{ 19, 23 },{ 21, 22 },{ 21, 23 },{ 15, 12 },
	{ 16, 12 },{ 17, 12 },{ 18, 12 },{ 19, 12 },{ 20, 12 },
	{ 21, 12 },{ 22, 12 },{ 23, 12 },{ 24, 12 },{ 25, 12 },
	{ 25, 13 },{ 25, 14 },{ 25, 15 },{ 25, 16 },{ 25, 17 },
	{ 25, 18 },{ 25, 19 },{ 25, 20 },{ 25, 21 },{ 24, 21 },
	{ 23, 21 },{ 22, 21 },{ 21, 21 },{ 25, 21 },{ 19, 21 },
	{ 18, 21 },{ 17, 21 },{ 16, 21 },{ 15, 21 },{ 15, 20 },
	{ 15, 19 },{ 15, 18 },{ 15, 17 },{ 15, 16 },{ 15, 15 },
	{ 15, 14 },{ 15, 13 },{ 18, 13 },{ 18, 14 },{ 18, 15 },
	{ 18, 16 },{ 18, 17 },{ 18, 18 },{ 19, 18 },{ 21, 18 },
	{ 22, 18 },{ 22, 13 },{ 16, 20 },{ 24, 20 },{ 20, 17 },
}


function make_solid(list)
	for i=1,#list do
		setTileSolid(list[i][1], list[i][2], true)
	end
end

function make_unsolid(list)
	for i=1,#list do
		setTileSolid(list[i][1], list[i][2], false)
	end
end


function change_layers(x1, y1, x2, y2, from, to)
	local x, y
	for y=y1,y2 do
		for x=x1,x2 do
			local v 
			v = getTileLayer(x, y, from)
			if (not (v == -1)) then
				setTileLayer(x, y, from, -1)
				setTileLayer(x, y, to, v)
			end
		end
	end
end

function lower()
	change_layers(16, 13, 24, 20, 2, 0)
	change_layers(16, 13, 24, 20, 3, 1)
	change_layers(20, 21, 20, 22, 3, 1)
end

function raise()
	change_layers(16, 13, 24, 20, 0, 2)
	change_layers(16, 13, 24, 20, 1, 3)
	change_layers(20, 21, 20, 22, 1, 3)
end


function start()
	hut1 = Portal:new{x=8, y=20}
	hut2 = Portal:new{x=8, y=9}
	hut3 = Portal:new{x=20, y=8}
	hut4 = Portal:new{x=33, y=9}
	hut5 = Portal:new{x=33, y=20}
	out = Portal:new{x=19, y=29, width=3, height=1}
	big = Portal:new{x=20, y=18}
	
	if (getMilestone(MS_MUTTRACE_LOWERED)) then
		make_solid(high_solids)
		make_unsolid(low_solids)
		lower()
	end
end

function stop()
end

function update(step)
	if (hut1:update()) then
		change_areas("mr_hut-1", 3, 7, DIRECTION_NORTH)
	elseif (hut2:update()) then
		change_areas("mr_hut-2", 3, 7, DIRECTION_NORTH)
	elseif (hut3:update()) then
		change_areas("mr_hut-3", 3, 7, DIRECTION_NORTH)
	elseif (hut4:update()) then
		change_areas("mr_hut-4", 3, 7, DIRECTION_NORTH)
	elseif (hut5:update()) then
		change_areas("mr_hut-5", 3, 7, DIRECTION_NORTH)
	elseif (out:update()) then
		doMap("muttrace", "map2")
	elseif (getMilestone(MS_MUTTRACE_LOWERED) and big:update()) then
		change_areas("mrbig", 7, 19, DIRECTION_NORTH)
	end

	local px, py = getObjectPosition(0)

	if (px == 20 and py == 23) then
		if (not stopped) then
			stopObject(0)
			stopped = true
		end
		if (not getMilestone(MS_MUTTRACE_LOWERED)) then
			make_solid(high_solids)
			make_unsolid(low_solids)
			lower()
			setMilestone(MS_MUTTRACE_LOWERED, true)
		end
	elseif (px == 20 and py == 24) then
		stopped = false
		if (getMilestone(MS_MUTTRACE_LOWERED)) then
			make_solid(low_solids)
			make_unsolid(high_solids)
			raise()
			setMilestone(MS_MUTTRACE_LOWERED, false)
		end
	end
end

function activate(activator, activated)
end

function collide(id1, id2)
end

