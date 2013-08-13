music = "underground.ogg"

enemies = {}
enemies[0] = "2Wolves"
enemies[1] = "1Hornet"
enemies[2] = "2Goos"
enemies[3] = "3Leechs"
enemies[4] = "2Leechs1Goo"

exceptions = {
	{ 3, 4 },
	{ 50, 21 },
	{ 45, 22 },
};

function start()
	entrance = Portal:new{x=3, y=4}
	portal2 = Portal:new{x=50, y=21}
	chest = Chest:new{x=17, y=25, anim_set="chest", milestone=MS_UNDERGROUND_0_CHEST, index=ITEM_BONE_HELMET}
	chest2 = Chest:new{x=22, y=2, anim_set="chest", milestone=MS_UNDERGROUND_0_CHEST_2, index=ITEM_CURE, quantity=3}

end

function stop()
end

function update(step)
	check_battle(30, enemies, exceptions)

	entrance:update()
	portal2:update()

	if (entrance.go_time) then
		change_areas("Underground_start", 4, 9, DIRECTION_NORTH);
	elseif (portal2.go_time) then
		change_areas("Underground_1", 7, 34, DIRECTION_NORTH);
	end

	px, py = getObjectPosition(0)
	if ((not getMilestone(MS_RIDER_JOINED)) and (not getMilestone(MS_RIDER_APPEARED)) and (px == 45) and (py == 22)) then
		stopObject(0)
		setMilestone(MS_RIDER_APPEARED, true)
		scriptifyPlayer()
		rider = Object:new{x=50, y=21, anim_set="Rider", person=true, solid=false}
		rider.scripted_events = {
			{ event_type=EVENT_CUSTOM, callback=face },
			{ event_type=EVENT_REST, delay=3000 },
			{ event_type=EVENT_WALK, dest_x=50, dest_y=22 },
			{ event_type=EVENT_WALK, dest_x=47, dest_y=22 },
			{ event_type=EVENT_SPEAK, text="Rider: You must be the friend of the wizard!\nEny: You mean Tiggy? How did you know that? Where is he? Is he Ok?\nRider: Your friend just burnt Seaside to the ground.\nRider: Then he and his gang of bandits took a boat and set sail...\nRider: You wouldn't happen to know where they went?\nEny: No... I mean, maybe to the eastern Kingdom.\nRider: Ok, then I'm off...\nEny: Are you going to save him?\nRider: Uhh... Yes...\n" },
			{ event_type=EVENT_WAIT_FOR_SPEECH },
			{ event_type=EVENT_WALK, dest_x=48, dest_y=22 },
			{ event_type=EVENT_SPEAK, text="Eny: Wait! I'll go with you!\nRider: * sigh *\nRider: Well, you seem like a strong fighter. I may need your help.\nEny: Ok, then let's get out of here!\n" },
			{ event_type=EVENT_WAIT_FOR_SPEECH },
			{ event_type=EVENT_WALK, dest_x=px, dest_y=py },
			{ event_type=EVENT_CUSTOM, callback=done_scene },
		}
	end

	if (getMilestone(MS_RIDER_APPEARED) and not getMilestone(MS_RIDER_JOINED)) then
		rider:update(step)
	end
end

function activate(activator, activated)
	if (activated == chest.id) then
		chest:activate()
	elseif (activated == chest2.id) then
		chest2:activate()
	end
end

function collide(id1, id2)
end

function face()
	setObjectDirection(0, DIRECTION_EAST)
end

function done_scene()
	removeObject(rider.id)
	addPartyMember("Rider")
	descriptifyPlayer()
	setMilestone(MS_RIDER_JOINED, true)
end

