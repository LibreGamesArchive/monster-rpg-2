music = "mountains.ogg"

enemies = {}
enemies[0] = "2Chompers"
enemies[1] = "2Slimes2Larva"
enemies[2] = "2Cubs2Larva"
enemies[3] = "1Cub1Chomper"
enemies[4] = "3Meatballs"
enemies[5] = "1Meatball1Wasp"
enemies[6] = "2Wasps"

exceptions = {
	{ 111, 15 },
	{  10, 82 },
};

function start()
	if (not getMilestone(MS_BEAT_WITCH)) then
		witch = Object:new{x=69, y=12, anim_set="Witch"}
	end
	chest0 = Chest:new{x=43, y=75, anim_set="chest", milestone=MS_MOUNTAIN_CHEST_0, index=ITEM_ELIXIR, quantity=1}
	chest1 = Chest:new{x=72, y=69, anim_set="chest", milestone=MS_MOUNTAIN_CHEST_1, index=ITEM_CURE2, quantity=1}
	chest2 = Chest:new{x=19, y=17, anim_set="chest", milestone=MS_MOUNTAIN_CHEST_2, index=ITEM_ROUNDS, quantity=30}
	chest3 = Chest:new{x=109, y=7, anim_set="chest", milestone=MS_MOUNTAIN_CHEST_3, index=ITEM_ELIXIR, quantity=1}
	chest4 = Chest:new{x=111, y=7, anim_set="chest", milestone=MS_MOUNTAIN_CHEST_4, index=ITEM_CURE2, quantity=1}

	portal = Portal:new{x=111, y=15}
	portal_start = Portal:new{x=10, y=82}
end

function stop()
end

function update(step)
	if (battle_started) then
		if (battleWon) then
			setMilestone(MS_BEAT_WITCH, true)
			removeObject(witch.id)
		end
	end

	check_battle(30, enemies, exceptions)

	portal:update()
	if (portal.go_time) then
		change_areas("mountain_descent0", 3, 6, DIRECTION_NORTH)
	elseif (portal_start:update()) then
		doMap("mountains")
	end
end

function activate(activator, activated)
	if (not getMilestone(MS_BEAT_WITCH)) then
		if (activated == witch.id) then
			--[[
			doDialogue("Witch: I knew you would come by this way... Your friend told me so.\nEny: Tiggy?! Where is he?\nWitch: Nevermind that, I'm his new best friend...\nWitch: Now you have to go back down the mountain... The hard way!\n", true)
			startBattle("1Witch2Cubs")
			battle_started = true
			]]--
			doDialogue("Witch: The demo ends here, Missy!\n", true)
		end
	end

	if (activated == chest0.id) then
		chest0:activate()
	elseif (activated == chest1.id) then
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

