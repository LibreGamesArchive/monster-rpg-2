music = "castle.caf"

function start()
	mainDoor = Door:new{x=22, y=32, anim_set="castledoors"}
	mainPortal = Portal:new{x=22, y=31, width=2, height=2}
	soldier1_1 = Object:new{x=20, y=44, anim_set="soldier", person=true}
	soldier1_2 = Object:new{x=25, y=44, anim_set="soldier", person=true}
	soldier2_1 = Object:new{x=21, y=33, anim_set="soldier", person=true}
	soldier2_2 = Object:new{x=24, y=33, anim_set="soldier", person=true}

	backPortal1 = Portal:new{x=18, y=25, width=1, height=1}
	backPortal2 = Portal:new{x=27, y=25, width=1, height=1}
	topLeftPortal = Portal:new{x=18, y=27, width=1, height=1}
	topRightPortal = Portal:new{x=27, y=27, width=1, height=1}
	leftSidePortal = Portal:new{x=13, y=31, width=1, height=1}
	rightSidePortal = Portal:new{x=32, y=31, width=1, height=1}

	tower0Portal = Portal:new{x=15, y=16, width=1, height=1}
	tower1Portal = Portal:new{x=9, y=34, width=1, height=1}
	tower2Portal = Portal:new{x=36, y=34, width=1, height=1}
	tower3Portal = Portal:new{x=15, y=44, width=1, height=1}
	tower4Portal = Portal:new{x=30, y=44, width=1, height=1}

	fruitTree = Object:new{x=22, y=4, anim_set="fruitTree"}
	setObjectHigh(fruitTree.id, true)

	chest0 = Chest:new{x=34, y=31, anim_set="chest", milestone=MS_CASTLE_CHEST0, index=ITEM_CURE}

	picture1 = Object:new{x=22, y=20}
	picture2 = Object:new{x=23, y=20}
end

function stop()
end

function update(step)
	px, py = getObjectPosition(0)
	if (py >= 52) then
		stopObject(0)
		doMap("castle")
	end

	mainPortal:update()

	if (mainDoor == nil and mainPortal.go_time) then
		change_areas("castle_l1", 17, 33, DIRECTION_NORTH)
	end

	backPortal1:update()

	if (backPortal1.go_time) then
		change_areas("castle_l1", 9, 17, DIRECTION_EAST)
	end

	backPortal2:update()

	if (backPortal2.go_time) then
		change_areas("castle_l1", 25, 17, DIRECTION_WEST)
	end

	topLeftPortal:update()
	topRightPortal:update()

	if (topLeftPortal.go_time) then
		change_areas("castle_l2", 1, 18, DIRECTION_NORTH)
	end

	if (topRightPortal.go_time) then
		change_areas("castle_l2", 17, 18, DIRECTION_NORTH)
	end

	leftSidePortal:update()
	rightSidePortal:update()

	if (leftSidePortal.go_time) then
		change_areas("castle_l1", 1, 25, DIRECTION_EAST)
	end
	if (rightSidePortal.go_time) then
		change_areas("castle_l1", 33, 25, DIRECTION_WEST)
	end

	tower0Portal:update()
	tower1Portal:update()
	tower2Portal:update()
	tower3Portal:update()
	tower4Portal:update()

	if (tower0Portal.go_time) then
		change_areas("castle_tower0", 4, 9, DIRECTION_NORTH)
	end
	if (tower1Portal.go_time) then
		change_areas("castle_tower1_1", 4, 9, DIRECTION_NORTH)
	end
	if (tower2Portal.go_time) then
		change_areas("castle_tower2_1", 4, 9, DIRECTION_NORTH)
	end
	if (tower3Portal.go_time) then
		change_areas("castle_tower3_1", 4, 9, DIRECTION_NORTH)
	end
	if (tower4Portal.go_time) then
		change_areas("castle_tower4_1", 4, 9, DIRECTION_NORTH)
	end
end

function activate(activator, activated)
	if (activated == soldier1_1.id) then
		smartDialogue{"Soldier: This armor is just for show. We live in peaceful times.\n",
			MS_BEACH_BATTLE_DONE, "Soldier: We heard about the monsters up north! I can't believe it...\n"}
	elseif (activated == soldier1_2.id) then
		smartDialogue{"Soldier: In times of peace, the King allows visitors.\n",
			MS_BEACH_BATTLE_DONE, "Soldier: The King is expecting you.\n"}
	elseif (activated == soldier2_1.id) then
		doDialogue("Soldier: The Throne Room is upstairs.\n")
	elseif (activated == soldier2_2.id) then
		doDialogue("Soldier: Stop by the kitchen if you're hungry!\n")
	elseif (activated == fruitTree.id) then
		if (getMilestone(MS_GOT_FRUIT)) then
			doDialogue("You already have some fruit...\n")
		else
			slot = findEmptyInventorySlot()
			if (slot < 0) then
				doDialogue("You can't hold anything else...\n")
			else
				loadPlayDestroy("chest.ogg")
				doDialogue("You got some fruit!\n")
				setInventory(slot, ITEM_FRUIT, 1)
				setMilestone(MS_GOT_FRUIT, true)
			end
		end
	elseif (activated == chest0.id) then
		chest0:activate()
	elseif (activated == picture1.id or activated == picture2.id) then
		doKingKingAlbertLook()
	end
end

function activate_any(tx, ty)
	local px, py = getObjectPosition(0)
	if (ty == 19 and py == 21 and ((px == 22 and tx == 22) or (px == 23 and tx == 23))) then
		setObjectDirection(0, DIRECTION_NORTH);
		doKingKingAlbertLook()
		return true
	end
	return false
end

function collide(id1, id2)
	if ((not (mainDoor == nil)) and ((id1 == 0 and id2 == mainDoor.id) or (id2 == 0 and id1 == mainDoor.id))) then
		mainDoor:open()
		removeObject(mainDoor.id)
		mainDoor = nil
	end
end

