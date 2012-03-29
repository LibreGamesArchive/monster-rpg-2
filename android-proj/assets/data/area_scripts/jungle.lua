music = "jungle_ambience.caf"
ambience = "jungle.flac"

enemies = {}
enemies[0] = "2Creeps"
enemies[1] = "2Stalkers"
enemies[2] = "3Gnomes"
enemies[3] = "1Gnome2Vipers"
enemies[4] = "1Millipede"
enemies[5] = "3Envys"
enemies[6] = "2Thornsters1Vinester1Rocky"
enemies[7] = "2Squitos"

exceptions = {
	{ 17, 98 },
	{ 17, 99 },
	{ 79, 0 },
	{ 79, 1 },
}


tree_battle_started = false


function start()
	if (getMilestone(MS_FREED_PRISONER)) then
		prisoner = Object:new{x=19, y=94, anim_set="prisoner", person=true}
		setObjectDirection(prisoner.id, DIRECTION_SOUTH)
	end

	chest1 = Chest:new{x=6, y=9, anim_set="blue_chest", milestone=MS_JUNGLE_CHEST_1, index=ITEM_JADE_BLADE}
	chest2 = Chest:new{x=85, y=42, anim_set="blue_chest", milestone=MS_JUNGLE_CHEST_2, index=ITEM_ELIXIR}
	chest3 = Chest:new{x=65, y=49, anim_set="blue_chest", milestone=MS_JUNGLE_CHEST_3, index=ITEM_JADE_CHEST_ARMOR}
	chest4 = Chest:new{x=107, y=62, anim_set="blue_chest", milestone=MS_JUNGLE_CHEST_4, index=ITEM_JADE_BLADE}
	chest5 = Chest:new{x=137, y=69, anim_set="blue_chest", milestone=MS_JUNGLE_CHEST_5, index=ITEM_HOLY_WATER}
	chest6 = Chest:new{x=120, y=90, anim_set="blue_chest", milestone=MS_JUNGLE_CHEST_6, index=ITEM_JADE_HELMET}
	chest7 = Chest:new{x=98, y=75, anim_set="blue_chest", milestone=MS_JUNGLE_CHEST_7, index=ITEM_JADE_STAFF}
	chest8 = Chest:new{x=29, y=79, anim_set="blue_chest", milestone=MS_JUNGLE_CHEST_8, itemtype=ITEM_GOLD, quantity=1000}

	tovillage = Portal:new{x=73, y=0, width=11, height=1}

	if (not getMilestone(MS_BEAT_TREE)) then
		tree = Object:new{x=116, y=3, anim_set="Possessed"}
	else
		chest = Chest:new{x=116, y=3, anim_set="blue_chest", milestone=MS_TREE_CHEST, itemtype=ITEM_GOLD, quantity=1000}
	end
end

function stop()
end


function update(step)
	check_battle(35, enemies, exceptions)

	if (tovillage:update()) then
		setMilestone(MS_PASSED_JUNGLE, true)
		doMap("jungle");
		return
	end

	local area_w = getAreaWidth()
	local area_h = getAreaHeight()
	local px, py = getObjectPosition(0)
	if (px == 0 or py == 0 or px == (area_w-1) or py == (area_h-1)) then
		doMap("jungle")
		return
	end
	
	if (tree_battle_started) then
		if (not inBattle()) then
			tree_battle_started = false
			descriptifyPlayer()
			if (battleWon()) then
				setMilestone(MS_BEAT_TREE, true)
				chest = Chest:new{x=116, y=3, anim_set="blue_chest", milestone=MS_TREE_CHEST, itemtype=ITEM_GOLD, quantity=1000}
				removeObject(tree.id)
			end
		end
	end

end

function activate(activator, activated)
	if (getMilestone(MS_FREED_PRISONER)) then
		if (activated == prisoner.id) then
			doDialogue("Thank you for freeing me from Ohleo Keep!\nThere is a village North East of here.\n")
		end
	end

	if (activated == chest1.id) then
		chest1:activate()
	elseif (activated == chest2.id) then
		chest2:activate()
	elseif (activated == chest3.id) then
		chest3:activate()
	elseif (activated == chest4.id) then
		chest4:activate()
	elseif (activated == chest5.id) then
		chest5:activate()
	elseif (activated == chest6.id) then
		chest6:activate()
	elseif (activated == chest7.id) then
		chest7:activate()
	elseif (activated == chest8.id) then
		chest8:activate()
	end
	
	if (not getMilestone(MS_BEAT_TREE)) then
		if (activated == tree.id) then
			scriptifyPlayer()
			doDialogue("Tree: Oh great, more visitors! Hrumph...\nTree: That's all I need! Bad enough that I'm possessed by a spirit!\nTree: Grrr... Well I'll just have to do away with you, it's not like I have any other choice!\n", true, false, true)
			startBattle("1Possessed", true)
			tree_battle_started = true
		end
	end

	if (not (chest == nil)) then
		if (activated == chest.id) then
			chest:activate()
		end
	end
end

function collide(id1, id2)
end

