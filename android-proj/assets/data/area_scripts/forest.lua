music = "forest.caf"

enemies = {}
enemies[0] = "2Shrooms1Bud"
enemies[1] = "3Coyotes"
enemies[2] = "3Hornets"
enemies[3] = "2Buds"
enemies[4] = "2Treants"

exceptions = {
	{ 41,62 },
	{ 42,62 },
	{ 43,62 },
	{ 44,62	},
	{ 45,62 },
	{ 46,62 },
	{ 47,62 },
	{ 28,39 },
	{ 29,40 },
	{ 13,32 },
};


function drop_mushrooms(o, e)
	setObjectAnimationSetPrefix(o.id, "")
end

function pickup_staff(o, e)
	removeObject(staff.id)
	setObjectAnimationSetPrefix(tig.id, "staff_")
	setObjectSubAnimation(tig.id, "stand_s")
end

function do_tint()
	tint(64, 128, 255, 0)
end

function first_battle()
	startBattle("first_battle", false)
	setGonnaFadeInRed(true)
	setObjectSubAnimation(0, "unconscious")
end


function fade_in()
	dpad_off()
	drawArea()
	dpad_on()
	fadeIn(255, 0, 0)
	setGonnaFadeInRed(false)
end

function summon()
	goblin1 = Object:new{x=69, y=64, anim_set="Goblin" }
	setObjectSubAnimation(goblin1.id, "smoke")
	goblin2 = Object:new{x=71, y=64, anim_set="Goblin" }
	setObjectSubAnimation(goblin2.id, "smoke")
end

function appear()
	setObjectSubAnimation(goblin1.id, "stand_s")
	setObjectSubAnimation(goblin2.id, "stand_s")
end

function fade_out()
	reviveAllPlayers()
	fadeOut(0, 0, 0, 5)
	clearBuffer(0, 0, 0)
	setMilestone(MS_OPENING_SCENE, true)
	startArea("Keep_cells")
	setObjectPosition(0, 13, 4)
	setObjectSubAnimation(0, "unconscious")
	updateArea()
	dpad_off()
	drawArea()
	dpad_on()
	fadeIn(0, 0, 0)
end

function raise()
	setObjectSubAnimation(goblin1.id, "hands_up")
	setObjectSubAnimation(goblin2.id, "hands_up")
end

function remove_horse()
	removeObject(horse.id)
end

function remove_rider()
	removeObject(rider.id)
end

function set_horse_milestone()
	setMilestone(MS_MET_HORSE, true)
end


function start()
	if (not getMilestone(MS_LOOKED_FROM_KEEP)) then
		tree = Object:new{x=35, y=19, anim_set="tree" }
		tree2 = Object:new{x=42, y=18, anim_set="tree" }
		tree3 = Object:new{x=39, y=19, anim_set="tree" }
		tree4 = Object:new{x=42, y=19, anim_set="tree" }
		tree5 = Object:new{x=43, y=17, anim_set="tree" }
	elseif (not getMilestone(MS_FOREST_GOLD)) then
		gold = Object:new{x=43, y=17, anim_set="gold"}
	end

	if (not getMilestone(MS_OPENING_SCENE)) then
		scriptifyPlayer()
		eny = Object:new{id=0, x=29, y=23}
		tig = Object:new{x=29, y=24, anim_set="Tiggy", person=true}
		eny.scripted_events = {
			{ event_type=EVENT_SPEAK, text="Eny: I think we got enough. \\ Tiggy: Yes, that should do...\n", top=true },
			{ event_type=EVENT_WAIT_FOR_SPEECH },
			{ event_type=EVENT_REST, delay=1000 },
			{ event_type=EVENT_GESTURE, name="shrug" },
			{ event_type=EVENT_REST, delay=1000 },
			{ event_type=EVENT_WALK, dest_x=28, dest_y=38 },
			{ event_type=EVENT_SPEAK, text="Eny: Wait for me!\nTiggy: Hurry up, slowpoke!\n" },
			{ event_type=EVENT_WAIT_FOR_SPEECH },
			{ event_type=EVENT_WALK, dest_x=28, dest_y=41 },
			{ event_type=EVENT_WALK, dest_x=31, dest_y=61 },
			{ event_type=EVENT_SPEAK, text="Eny: Tig!! Please slow down!\nTiggy: I want to get home and cook some of those mushrooms!\n" },
			{ event_type=EVENT_WAIT_FOR_SPEECH },
			{ event_type=EVENT_WALK, dest_x=40, dest_y=62 },
			{ event_type=EVENT_WALK, dest_x=49, dest_y=62 },
			{ event_type=EVENT_GESTURE, name="pant" },
			{ event_type=EVENT_SYNC, who=tig, number=14 },
			{ event_type=EVENT_WAIT_FOR_SPEECH },
			{ event_type=EVENT_REST, delay=1000 },
			{ event_type=EVENT_SPEAK, text="Eny: Tiggy!! Come back!\n" },
			{ event_type=EVENT_WAIT_FOR_SPEECH },
			{ event_type=EVENT_SPAWN, what={x=50, y=62, anim_set="mushroom_bag", solid=false} },
			{ event_type=EVENT_CUSTOM, callback=drop_mushrooms },
			{ event_type=EVENT_WALK, dest_x=49, dest_y=66 },
			{ event_type=EVENT_WALK, dest_x=70, dest_y=66 },
			{ event_type=EVENT_WALK, dest_x=70, dest_y=65 },
			{ event_type=EVENT_LOOK, direction=DIRECTION_NORTH },
			{ event_type=EVENT_SPEAK, text="Eny: Wow... what is it?\nTiggy: Some kind of wizard's staff... do you think?\nEny: No, it couldn't be...\n", bottom=true },
			{ event_type=EVENT_WAIT_FOR_SPEECH },
			{ event_type=EVENT_SYNC, who=tig, number=22 },
			{ event_type=EVENT_SPEAK, text="Eny: Tig... what are you doing?...\nEny: Tiggy?...\n", bottom=true },
			{ event_type=EVENT_CUSTOM, callback=do_tint },
			{ event_type=EVENT_WAIT_FOR_SPEECH },
			{ event_type=EVENT_CUSTOM, callback=first_battle },
			{ event_type=EVENT_WAIT_FOR_BATTLE },
			{ event_type=EVENT_CUSTOM, callback=fade_in },
			{ event_type=EVENT_SYNC, who=tig, number=39 },
			--{ event_type=EVENT_DESCRIPTIFY },
		}
		tig.scripted_events = {
			{ event_type=EVENT_LOOK, direction=DIRECTION_NORTH },
			{ event_type=EVENT_WAIT_FOR_SPEECH },
			{ event_type=EVENT_WALK, dest_x=29, dest_y=43 },
			{ event_type=EVENT_SYNC, who=eny, number=7 },
			{ event_type=EVENT_WAIT_FOR_SPEECH },
			{ event_type=EVENT_WALK, dest_x=31, dest_y=61 },
			{ event_type=EVENT_WALK, dest_x=36, dest_y=62 },
			{ event_type=EVENT_GESTURE, name="stand_s" },
			{ event_type=EVENT_SYNC, who=eny, number=11 },
			{ event_type=EVENT_WAIT_FOR_SPEECH },
			{ event_type=EVENT_WALK, dest_x=50, dest_y=62 },
			{ event_type=EVENT_WALK, dest_x=52, dest_y=66 },
			{ event_type=EVENT_LOOK, direction=DIRECTION_EAST },
			{ event_type=EVENT_SPEAK, text="Tiggy: Hey... what's that over there?\nEny: Come on, we have to get home before dark!\nTiggy: Just one minute, I want to see what it is...\n" },
			{ event_type=EVENT_WAIT_FOR_SPEECH },
			{ event_type=EVENT_WALK, dest_x=70, dest_y=66 },
			{ event_type=EVENT_WALK, dest_x=70, dest_y=63 },
			{ event_type=EVENT_LOOK, direction=DIRECTION_SOUTH },
			{ event_type=EVENT_SYNC, who=eny, number=27 },
			{ event_type=EVENT_WAIT_FOR_SPEECH },
			{ event_type=EVENT_WALK, dest_x=70, dest_y=64 },
			{ event_type=EVENT_CUSTOM, callback=pickup_staff },
			{ event_type=EVENT_SYNC, who=eny, number=30 },
			{ event_type=EVENT_WAIT_FOR_SPEECH },
			{ event_type=EVENT_SYNC, who=eny, number=33 },
			{ event_type=EVENT_WAIT_FOR_BATTLE },
			{ event_type=EVENT_SYNC, who=eny, number=35 },
			{ event_type=EVENT_REST, delay=1000 },
			{ event_type=EVENT_GESTURE, name="cast" },
			{ event_type=EVENT_CUSTOM, callback=summon },
			{ event_type=EVENT_REST, delay=2000 },
			{ event_type=EVENT_CUSTOM, callback=appear },
			{ event_type=EVENT_GESTURE, name="stand_s" },
			{ event_type=EVENT_SPEAK, text="Tiggy: Take her to Ohleo Keep, to the north of here.\nTiggy: ... and prepare for war!\n", bottom=true },
			{ event_type=EVENT_WAIT_FOR_SPEECH },
			{ event_type=EVENT_CUSTOM, callback=raise },
			{ event_type=EVENT_SPEAK, text="Goblins: Yes, Master!\n", bottom=true },
			{ event_type=EVENT_WAIT_FOR_SPEECH },
			{ event_type=EVENT_CUSTOM, callback=fade_out },
		}
		staff = Object:new{x=70, y=64, anim_set="staff"}
		setObjectSolid(staff.id, false)
	elseif (not getMilestone(MS_MET_HORSE)) then
		scriptifyPlayer()
		eny = Object:new{id=0, x=13, y=32}
		rider = Object:new{x=13, y=32, anim_set="Rider", person=true, solid=false}
		horse = Object:new{x=28, y=33, anim_set="dark_horse", person=true, high=true}
		horse.scripted_events = {
			{ event_type=EVENT_SYNC, who=rider, number=6 },
			{ event_type=EVENT_WALK, dest_x=28, dest_y=42 },
			{ event_type=EVENT_CUSTOM, callback=remove_horse },
		}
		rider.scripted_events = {
			{ event_type=EVENT_LOOK, direction=DIRECTION_EAST },
			{ event_type=EVENT_WALK, dest_x=14, dest_y=32 },
			{ event_type=EVENT_WALK, dest_x=14, dest_y=33 },
			{ event_type=EVENT_WALK, dest_x=26, dest_y=33 },
			{ event_type=EVENT_SYNC, who=eny, number=6 },
			{ event_type=EVENT_WAIT_FOR_SPEECH },
			{ event_type=EVENT_WALK, dest_x=25, dest_y=33 },
			{ event_type=EVENT_CUSTOM, callback=remove_rider },
		}
		eny.scripted_events = {
			{ event_type=EVENT_SYNC, who=rider, number=4 },
			{ event_type=EVENT_LOOK, direction=DIRECTION_EAST },
			{ event_type=EVENT_WALK, dest_x=14, dest_y=32 },
			{ event_type=EVENT_WALK, dest_x=14, dest_y=33 },
			{ event_type=EVENT_WALK, dest_x=25, dest_y=33 },
			{ event_type=EVENT_SPEAK, text="Rider: This is my horse. He's old so he can't carry us both.\nRider: Get going!\n" },
			{ event_type=EVENT_WAIT_FOR_SPEECH },
			{ event_type=EVENT_REST, delay=2000 },
			{ event_type=EVENT_SYNC, who=rider, number=8 },
			{ event_type=EVENT_DESCRIPTIFY },
			{ event_type=EVENT_CUSTOM, callback=set_horse_milestone },
		}
	end

	portal_to_underground = Portal:new{x=13, y=32}
end

function stop()
end

function update(step)
	if (not getMilestone(MS_OPENING_SCENE)) then
		eny:update(step)
		tig:update(step)
	elseif (not getMilestone(MS_MET_HORSE)) then
		eny:update(step)
		rider:update(step)
		horse:update(step)
	end

	portal_to_underground:update()

	if (portal_to_underground.go_time) then
		change_areas("Underground_2", 34, 10, DIRECTION_SOUTH);
	elseif (getMilestone(MS_MET_HORSE)) then
		px, py = getObjectPosition(0)
		if (py <= 12 or py >= 77) then
			stopObject(0)
			doMap("forest")
			return
		end
		check_battle(30, enemies, exceptions)
	end
end

function activate(activator, activated)
	if (not (gold == nil) and activated == gold.id) then
		loadPlayDestroy("chest.ogg")
		giveGold(1000)
		doDialogue("You found 1000 gold!\n", true)
		setMilestone(MS_FOREST_GOLD, true)
		removeObject(gold.id)
		gold = nil
	end
end

function collide(id1, id2)
end

