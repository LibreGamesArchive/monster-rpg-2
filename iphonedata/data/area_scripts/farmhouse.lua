music = "farmer.ogg"

local scene_started = false

function start()
	up = Portal:new{x=1, y=5}
	out = Portal:new{x=7, y=11}

	chest = Chest:new{x=3, y=3, milestone=MS_FARMERS_POT1, itemtype=ITEM_GOLD, quantity=500}

	if (not getMilestone(MS_FARMER_GONE_TO_GATE)) then
		farmer = Object:new{x=13, y=5, anim_set="Farmer", person=true}
		setObjectDirection(farmer.id, DIRECTION_SOUTH)
	end
end

function stop()
end

function update(step)
	if (up:update()) then
		change_areas("farmhouse2", 1, 5, DIRECTION_EAST)
	elseif (out:update()) then
		change_areas("farm", 27, 23, DIRECTION_SOUTH)
	end

	if (scene_started) then
		farmer:update(step)
	end
end

function remove_farmer()
	setMilestone(MS_FARMER_GONE_TO_GATE, true)
	removeObject(farmer.id)
	descriptifyPlayer()
	scene_started = false
end

function activate(activator, activated)
	if (activated == chest.id) then
		chest:activate()
	elseif (farmer and activated == farmer.id) then
		setObjectDirection(farmer.id, player_dir(farmer))
		if (
				getMilestone(MS_COW1) and
				getMilestone(MS_COW2) and
				getMilestone(MS_COW3) and
				getMilestone(MS_COW4)) then
			if (getMilestone(MS_GOT_MILK)) then
				if (getMilestone(MS_BEAT_GIRL_DRAGON)) then
					scriptifyPlayer()
					doDialogue("Farmer: Oh hey, did my ol' buddy Gunnar tell ya's about my rocket ship? Come on outside, and I'll show her to ya's!\n", true)
					setObjectSolid(farmer.id, false)
					farmer.scripted_events = {
						{ event_type=EVENT_WALK, dest_x=7, dest_y=5 },
						{ event_type=EVENT_WALK, dest_x=7, dest_y=11 },
						{ event_type=EVENT_CUSTOM, callback=remove_farmer },
					}
					scene_started = true
				else
					doDialogue("Farmer: Now now, ya's had your milk, don't be greedy HYAHAHAHA...\n", true)
				end
			else
				doDialogue("Farmer: I seen how nice you were to my cows... gosh, I gotta tell ya, I really appreciate it!\nFarmer: Here, have some milk, it's on me!\n", true)
				allPointsUp(100)
				setMilestone(MS_GOT_MILK, true)
			end
		else
			doDialogue("Farmer: Hey fellers! Nice to see ya's again!\nFarmer: Hey, say hi to my cows while you're here!\n", true)
		end
		setObjectDirection(farmer.id, DIRECTION_SOUTH)
	end
end

function collide(id1, id2)
end

