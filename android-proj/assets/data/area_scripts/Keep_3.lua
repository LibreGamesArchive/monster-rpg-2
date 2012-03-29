music = "keep.caf"

enemies = {}
enemies[0] = "2Mages1Halberd"
enemies[1] = "1Mage2Stabbers"
enemies[2] = "1Beast"
enemies[3] = "3Rages"

exceptions = {
	{14,3},
	{14,12},
	{13,20},
	{14,20},
}


function start()
	down = Portal:new{x=14, y=12}
	up = Portal:new{x=14, y=3}
	out = Portal:new{x=13, y=20, width=2}
	
	chest0 = Chest:new{x=24, y=6, anim_set="chest", milestone=MS_KEEP_3_CHEST_0, index=ITEM_CURE3}
	chest1 = Chest:new{x=24, y=18, anim_set="chest", milestone=MS_KEEP_3_CHEST_1, index=ITEM_EMERALD_STAFF}
	chest2 = Chest:new{x=3, y=18, anim_set="chest", milestone=MS_KEEP_3_CHEST_2, index=ITEM_EMERALD_ROBE}
	chest3 = Chest:new{x=3, y=6, anim_set="chest", milestone=MS_KEEP_3_CHEST_3, index=ITEM_CURE3}

	door_out = Door:new{x=13, y=20, width=2, anim_set="Keep_mid_door"}
	door_up = Door:new{x=13, y=6, width=2, anim_set="Keep_mid_door"}
end

function stop()
end

function update(step)
	check_battle(30, enemies, exceptions)

	if (down:update()) then
		change_areas("Keep_2", 19, 12, DIRECTION_SOUTH)
	elseif(up:update()) then
		change_areas("Keep_4", 14, 3, DIRECTION_SOUTH)
	elseif (out:update()) then
		change_areas("Keep_outer", 19, 21, DIRECTION_SOUTH)
	end
end

function activate(activator, activated)
	if (activated == chest0.id) then
		chest0:activate()
	elseif (activated == chest1.id) then
		chest1:activate()
	elseif (activated == chest2.id) then
		chest2:activate()
	elseif (activated == chest3.id) then
		chest3:activate()
	end
end

function collide(id1, id2)
	if ((not (door_out == nil)) and (id1 == door_out.id or id2 == door_out.id)) then
		if (getMilestone(MS_KEEP_2_CHEST_1)) then
			-- take key
			local slot, quantity = findUsedInventorySlot(ITEM_KEEP_KEY_OUT)
			if (slot >= 0) then
				setInventory(slot, -1, 0)
			end
			door_out:open()
			removeObject(door_out.id)
			door_out = nil
		else
			doDialogue("Locked...\n")
		end
	elseif ((not (door_up == nil)) and (id1 == door_up.id or id2 == door_up.id)) then
		if (getMilestone(MS_NANNER_CHEST)) then
			-- take key
			local slot, quantity = findUsedInventorySlot(ITEM_KEEP_KEY_UP)
			if (slot >= 0) then
				setInventory(slot, -1, 0)
			end
			door_up:open()
			removeObject(door_up.id)
			door_up = nil
		else
			doDialogue("Locked...\n")
		end
	end
end

