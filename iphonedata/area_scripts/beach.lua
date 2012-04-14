if (not getMilestone(MS_BEACH_BATTLE_DONE)) then
	music = "beach.ogg"
else
	music = "beach_atmosphere.ogg"
end


first_update = true
do_rios_thing = false

forward = {}

curr_battle = 0

battles = {
	"1Goofball",
	"1Sludge",
	"1Imp",
	"1Harpy",
	"2Goofballs",
	"2Sludges",
	"2Imps",
	"2Harpys",
	"3Goofballs",
	"1Goofball2Sludges",
	"3Imps",
	"1Imp2Harpys",
	"1Stomper2Imps",
}

backs = {
	{
		{ x=8, y=8, anim_set="Goofball" },
	},
	{
		{ x=8, y=8, anim_set="Sludge" },
	},
	{
		{ x=8, y=8, anim_set="Imp" },
	},
	{
		{ x=8, y=8, anim_set="Harpy" },
	},
	{
		{ x=8, y=8, anim_set="Goofball" },
		{ x=9, y=8, anim_set="Goofball" },
	},
	{
		{ x=8, y=8, anim_set="Sludge" },
		{ x=9, y=8, anim_set="Sludge" },
	},
	{
		{ x=8, y=8, anim_set="Imp" },
		{ x=9, y=8, anim_set="Imp" },
	},
	{
		{ x=8, y=8, anim_set="Harpy" },
		{ x=9, y=8, anim_set="Harpy" },
	},
	{
		{ x=7, y=8, anim_set="Goofball" },
		{ x=8, y=8, anim_set="Goofball" },
		{ x=9, y=8, anim_set="Goofball" },
	},
	{
		{ x=7, y=8, anim_set="Goofball" },
		{ x=8, y=8, anim_set="Sludge" },
		{ x=9, y=8, anim_set="Sludge" },
	},
	{
		{ x=7, y=8, anim_set="Imp" },
		{ x=8, y=8, anim_set="Imp" },
		{ x=9, y=8, anim_set="Imp" },
	},
	{
		{ x=7, y=8, anim_set="Imp" },
		{ x=8, y=8, anim_set="Harpy" },
		{ x=9, y=8, anim_set="Harpy" },
	},
	{
		{ x=7, y=8, anim_set="Imp" },
		{ x=8, y=8, anim_set="Stomper" },
		{ x=9, y=8, anim_set="Imp" },
	},
}

deads = {
	{
		{ x=2, y=6 },
	},
	{
		{ x=10, y=6 },
	},
	{
		{ x=3, y=6 },
	},
	{
		{ x=11, y=6 },
	},
	{
		{ x=4, y=6 },
		{ x=12, y=6 },
	},
	{
		{ x=5, y=6 },
		{ x=13, y=6 },
	},
	{
		{ x=6, y=6 },
		{ x=14, y=6 },
	},
	{
		{ x=2, y=7 },
		{ x=10, y=7 },
	},
	{
		{ x=3, y=7 },
		{ x=11, y=7 },
		{ x=4, y=7 },
	},
	{
		{ x=12, y=7 },
		{ x=5, y=7 },
		{ x=13, y=7 },
	},
	{
		{ x=6, y=7 },
		{ x=14, y=7 },
		{ x=2, y=8 },
	},
	{
		{ x=10, y=8 },
		{ x=3, y=8 },
		{ x=11, y=8 },
	},
	{
		{ x=4, y=8 },
		{ x=12, y=8 },
		{ x=5, y=8 },
	},
}

battle_started = false

function done()
	removeObject(rios.id)
	removePlayer("Rios")
	setMilestone(MS_RIOS_LEFT, true)
	--descriptifyPlayer();
	do_rios_thing = false
	setObjectDirection(0, DIRECTION_SOUTH)
	for i=1,12 do
		if (i <= 4 or i >= 9) then
			setObjectSubAnimation(dudes[i].obj.id, "stand_s")
		end
	end
	setObjectSubAnimation(0, "stand_s")
end

function next_enemy()
	curr_battle = curr_battle + 1
	for i=1,#backs[curr_battle] do
		backs[curr_battle][i].obj = Object:new(backs[curr_battle][i])
	end
end


function end_enemy()
	for i=1,#backs[curr_battle] do
		setObjectSubAnimation(backs[curr_battle][i].obj.id, "dead")
		setObjectPosition(backs[curr_battle][i].obj.id,
			deads[curr_battle][i].x,
			deads[curr_battle][i].y)
		--removeObject(backs[curr_battle][i].obj.id)
	end
end


function do_info(spot, pos)
	if (pos == -1) then
		showBeachBattleInfo(spot)
	else
		local x, y = getObjectPosition(dudes[spot+1].obj.id)
		if ((y-2) == pos) then
			showBeachBattleInfo(spot)
		end
	end
end


function setManSubAnim(spot, name)
	setObjectSubAnimation(dudes[spot+1].obj.id, name)
end

function move_player(spot, pos)
	if (spot <= 5) then
		spot = spot + 1
	end

	local x, y = getObjectPosition(dudes[spot].obj.id)

	if (#forward >= 4 and not (y > 2)) then
		loadPlayDestroy("error.ogg")
		return
	end

	if (pos == 0) then
		for i=1,4 do
			if (forward[i] == spot) then
				forward[i] = nil
				break
			end
		end
		local i1 = 1
		local i2 = 1
		repeat
			forward[i1] = forward[i2]
			if (not (forward[i1] == nil)) then
				i1 = i1 + 1
			end
			i2 = i2 + 1
		until (i2 > 4)
		forward[i1] = nil
	elseif (y == 2) then
		forward[#forward+1] = spot
	end

	setObjectPosition(dudes[spot].obj.id, x, 2+pos)

	loadPlayDestroy("select.ogg")
end


function toggle(spot)
	if (spot <= 5) then
		spot = spot + 1
	end

	local x, y = getObjectPosition(dudes[spot].obj.id)

	if (y == 2) then
		if (#forward >= 4) then
			loadPlayDestroy("error.ogg")
			return
		end

		setObjectPosition(dudes[spot].obj.id, x, y+1)
		forward[#forward+1] = spot
	elseif (y == 3) then
		setObjectPosition(dudes[spot].obj.id, x, y+1)
	else
		setObjectPosition(dudes[spot].obj.id, x, y-2)
		for i=1,4 do
			if (forward[i] == spot) then
				forward[i] = nil
				break
			end
		end
		local i1 = 1
		local i2 = 1
		repeat
			forward[i1] = forward[i2]
			if (not (forward[i1] == nil)) then
				i1 = i1 + 1
			end
			i2 = i2 + 1
		until (i2 > 4)
		forward[i1] = nil
		
	end

	loadPlayDestroy("select.ogg")
end


function go()
	if (#forward < 1) then
		loadPlayDestroy("error.ogg")
		return 1
	end

	clearPlayers()

	for i=1,#forward do
		local x, y = getObjectPosition(dudes[forward[i]].obj.id)
		local n
		if (forward[i] > 6) then
			n = forward[i]
		else
			n = forward[i]-1
		end
		markMan(n, true, false)
		dudes[forward[i]].used = true
		if (dudes[forward[i]].anim_set == "Guard") then
			if (forward[i] > 6) then
				addGuard(forward[i]-4-1)
			else
				addGuard(forward[i]-1)
			end
		else
			addPlayer(dudes[forward[i]].anim_set)
		end
		setObjectPosition(dudes[forward[i]].obj.id, x, 2)
		if (y == 3) then
			setPlayerFormation(i-1, FORMATION_BACK)
		else
			setPlayerFormation(i-1, FORMATION_FRONT)
		end
	end

	startBattle(battles[curr_battle], false)

	battle_started = true

	return 0
end


function start()
	addFish(68, 240);
	addFish(204, 240);

	out = Portal:new{x=8, y=0}

	if ((not getMilestone(MS_BEACH_BATTLE_DONE)) or (not getMilestone(MS_GUARDS_LEFT_BEACH))) then
		dudes = {
			{ x=2, y=2, anim_set="Guard", person=true, used=false },
			{ x=3, y=2, anim_set="Guard", person=true, used=false },
			{ x=4, y=2, anim_set="Guard", person=true, used=false },
			{ x=5, y=2, anim_set="Guard", person=true, used=false },
			{ x=6, y=2, anim_set="Eny", person=true, used=false },
			{ x=7, y=2, anim_set="Rider", person=true, used=false },
			{ x=9, y=2, anim_set="Rios", person=true, used=false },
			{ x=10, y=2, anim_set="Gunnar", person=true, used=false },
			{ x=11, y=2, anim_set="Guard", person=true, used=false },
			{ x=12, y=2, anim_set="Guard", person=true, used=false },
			{ x=13, y=2, anim_set="Guard", person=true, used=false },
			{ x=14, y=2, anim_set="Guard", person=true, used=false },
		}

		if (getMilestone(MS_BEACH_BATTLE_DONE)) then
			for i = 1,12 do
				if (dudes[i].anim_set == "Guard") then
					dudes[i].obj = Object:new(dudes[i])
				end
			end
		else
			pushPlayers()
			reviveAllPlayers()
			scriptifyPlayer()
			followPlayer(false)
			setAreaFocus((getAreaWidth()*TILE_SIZE)/2, BH/2)
			setAreaPan(-BW/2, -BH/2)
			setObjectHidden(0, true);
			for i = 1,12 do
				dudes[i].obj = Object:new(dudes[i])
				if (i == 5) then
					setObjectDirection(dudes[i].obj.id, DIRECTION_WEST)
				elseif (i == 4) then
					setObjectDirection(dudes[i].obj.id, DIRECTION_EAST)
				else
					setObjectDirection(dudes[i].obj.id, DIRECTION_SOUTH)
				end
			end
		end
	end
end

function stop()
end

function update(step)
	if (first_update == true) then
		first_update = false
		return
	end

	if (do_rios_thing) then
		rios:update(step)
	end

	if (out:update()) then
		doMap("beach")
	end

	if (not getMilestone(MS_BEACH_INTRO)) then
		setMilestone(MS_BEACH_INTRO, true)
		doDialogue("Boy am I glad you're here. A messenger said you were coming. The enemy keeps pounding us from the water.\nWe need to split up in groups of 1 to 4 and attack the enemies strategically.\nPosition your weaker members in the back and stronger ones in the front.\n", true, false, true)
		setObjectDirection(dudes[4].id, DIRECTION_SOUTH)
		setObjectDirection(dudes[5].id, DIRECTION_SOUTH)

		addManChooser(13,
			2, 1, false,
			3, 1, false,
			4, 1, false,
			5, 1, false,
			6, 1, false,
			7, 1, false,
			8, 1, true,
			9, 1, false,
			10, 1, false,
			11, 1, false,
			12, 1, false,
			13, 1, false,
			14, 1, false
		)

		createGuards()

		next_enemy()
	end


	if (battle_started and not inBattle()) then
		-- mark dead
		for i=1,#forward do
			if (battleLost() or battleIsDead(i-1)) then
				local n
				if (forward[i] > 6) then
					n = forward[i]
					dudes[n].dead = true
				else
					n = forward[i]-1
					dudes[n+1].dead = true
				end
				markMan(n, false, true)
			end
		end

		-- check if all are dead
		local all_dead = true
		for i=1,#dudes do
			if (not dudes[i].dead) then
				all_dead = false
				break
			end
		end

		if (all_dead) then
			gameOver()
			return
		end

		forward = {}

		popPlayersNoClear()

		-- Mark all unused if all are used
		local all_used = true

		for i=1,#dudes do
			if ((not dudes[i].dead) and dudes[i].used == false) then
				all_used = false
				break
			end
		end

		if (all_used) then
			for i=1,#dudes do
				dudes[i].used = false
				local n
				if (i > 6) then
					n = i
				else
					n = i-1
				end
				markMan(n, false, dudes[i].dead)
			end
		end

		-- check for end

		if (battleWon()) then
			end_enemy()
			if (curr_battle == #battles) then
				clearPushedPlayers()
				playMusic("beach_atmosphere.ogg")
				setMilestone(MS_BEACH_BATTLE_DONE, true)
				descriptifyPlayer()
				followPlayer(true)
				setAreaPan(-5*TILE_SIZE-TILE_SIZE/2,
					-2*TILE_SIZE-TILE_SIZE/2)
				reviveAllPlayers()
				destroyManChooser()	
				destroyGuards()
				for i=1,#dudes do
					if (not (dudes[i].anim_set == "Guard")) then
						removeObject(dudes[i].obj.id)
					end
				end
				setObjectPosition(0, 6, 2);
				setObjectHidden(0, false)
				setObjectSolid(0, true)
				setObjectDirection(0, DIRECTION_EAST)

				rios = Object:new{x=7, y=2, anim_set="Rios", person=true}
				setObjectDirection(rios.id, DIRECTION_WEST)
				do_rios_thing = true

				rios.scripted_events = {
					{ event_type=EVENT_SPEAK, text="Rios: Well, I guess I have to honor my promise... to go back to Flowey and recreate the history of Seaside...\nEny: Take good care, perhaps we will see you again!\nRios: I think that is highly likely!\nEny: Goodbye...\n" },
					{ event_type=EVENT_WAIT_FOR_SPEECH },
					{ event_type=EVENT_WALK, dest_x=8, dest_y=2 },
					{ event_type=EVENT_WALK, dest_x=8, dest_y=0 },
					{ event_type=EVENT_CUSTOM, callback=done },
				}
			else
				next_enemy()
			end
		end

		if (not getMilestone(MS_BEACH_BATTLE_DONE)) then
			showManChooser()
		end

		battle_started = false
	end
end

function activate(activator, activated)
	if (activated == dudes[1].obj.id) then
		doDialogue("I've never been part of such a big battle... it was kind of fun!\n")
	elseif (activated == dudes[2].obj.id) then
		doDialogue("I want to go home and get some sleep!\n")
	elseif (activated == dudes[3].obj.id) then
		doDialogue("I think we can all agree, we saved your skins out here...\n")
	elseif (activated == dudes[4].obj.id) then
		doDialogue("Well, the hard work is done now...\n")
	elseif (activated == dudes[9].obj.id) then
		doDialogue("Hey, never knew that 'magic' stuff would come in handy.\n")
	elseif (activated == dudes[10].obj.id) then
		doDialogue("So I guess you'll bring word of this to the King?\n")
	elseif (activated == dudes[11].obj.id) then
		doDialogue("WAM! BAM! I was ON FIRE!!!\n")
	elseif (activated == dudes[12].obj.id) then
		doDialogue("You didn't underestimate the power of the Flowey Guard did you? Hehehehahaha!\n")
	else
		return
	end

	setMilestone(MS_GUARDS_LEFT_BEACH, true);
end

function collide(id1, id2)
end

