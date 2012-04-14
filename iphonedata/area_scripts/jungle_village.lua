if (get_platform() == "iphone") then
	music = "jungle_ambience.flac"
	ambience = "fire.flac"
else
	music = "jungle_ambience.ogg"
	ambience = "fire.ogg"
end

function start()
	fire = Object:new{x=9, y=20, anim_set="fire", solid=false}
	
	tohut1 = Portal:new{x=7, y=14}
	tohut2 = Portal:new{x=23, y=14}
	tohut3 = Portal:new{x=31, y=17}

	woman1 = Object:new{x=8, y=20, anim_set="villager7", person=true, move_type=MOVE_NONE}
	setObjectDirection(woman1.id, DIRECTION_SOUTH)
	girl1 = Object:new{x=15, y=17, anim_set="villager6", person=true, move_type=MOVE_WANDER}
	girl2 = Object:new{x=30, y=25, anim_set="villager4", person=true, move_type=MOVE_WANDER}
	boy = Object:new{x=26, y=20, anim_set="villager3", person=true, move_type=MOVE_WANDER}
	woman2 = Object:new{x=28, y=18, anim_set="villager2", person=true, move_type=MOVE_NONE}
	setObjectDirection(woman2.id, DIRECTION_SOUTH)
	man = Object:new{x=4, y=18, anim_set="villager1", person=true, move_type=MOVE_WANDER}

	chest = Chest:new{x=19, y=18, anim_set="blue_chest", milestone=MS_JUNGLE_VILLAGE_CHEST, index=ITEM_ONYX_SWORD}
end

function stop()
	set_ambience_volume(math.itofix(1))
end

function update(step)
	local px, py = getObjectPosition(0)
	local w = getAreaWidth()
	local h = getAreaHeight()

	if (px == 0 or px == (w-1) or py == (h-1)) then
		doMap("jungle_village")
	elseif (tohut1:update()) then
		change_areas("hut1", 3, 7, DIRECTION_NORTH)
	elseif (tohut2:update()) then
		change_areas("hut2", 3, 7, DIRECTION_NORTH)
	elseif (tohut3:update()) then
		change_areas("hut3", 3, 7, DIRECTION_NORTH)
	elseif (py == 0) then
		setMilestone(MS_UNLOCKED_VOLCANO, true)
		doMap("jungle_village")
	end

	woman1:move(step)
	woman2:move(step)
	girl1:move(step)
	girl2:move(step)
	boy:move(step)
	man:move(step)

	-- Positional audio for the fire
	local px, py = getObjectPosition(0)
	local distx = math.fixtoi(math.fixabs(math.itofix(px - 10)))
	local disty = math.fixtoi(math.fixabs(math.itofix(py - 20)))
	local dist = math.fixsqrt(math.itofix(distx*distx+disty*disty))
	if (math.fixtoi(dist) >= 10) then
		set_ambience_volume(math.itofix(0))
	else
		local vol = math.fixsub(math.itofix(1), math.fixdiv(dist, math.itofix(10)))
		set_ambience_volume(vol)
	end
end

function activate(activator, activated)
	if (activated == girl1.id) then
		setObjectDirection(girl1.id, player_dir(girl1))
		doDialogue("Girl: I'm the most beautiful girl in the jungle...\n")
	elseif (activated == woman1.id) then
		local _d = getObjectDirection(woman1.id)
		setObjectDirection(woman1.id, player_dir(woman1))
		doDialogue("Woman: This fire must be kept going, or the jungle predators will get too close...\n", true)
		setObjectDirection(woman1.id, _d)
	elseif (activated == man.id) then
		setObjectDirection(man.id, player_dir(man))
		doDialogue("Man: We have lots of food... but fishing is so relaxing!\n")
	elseif (activated == girl2.id) then
		setObjectDirection(girl2.id, player_dir(girl2))
		doDialogue("Girl: Welcome to our village. We've been here since the volcano erupted last... that was a long long time ago.\n")
	elseif (activated == boy.id) then
		setObjectDirection(boy.id, player_dir(boy))
		doDialogue("Boy: Some day I'm going to leave the jungle and become a great warrior!\n")
	elseif (activated == woman2.id) then
		local _d = getObjectDirection(woman2.id)
		setObjectDirection(woman2.id, player_dir(woman2))
		doDialogue("Woman: It's such a peaceful place, as long as there is a fire burning... Feel free to get some rest inside.\n", true)
		setObjectDirection(woman2.id, _d)
	elseif (activated == chest.id) then
		chest:activate()
	end
end

function collide(id1, id2)
end

