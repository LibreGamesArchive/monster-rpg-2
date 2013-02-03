music = "castle.ogg"

function start()
	down = Portal:new{x=1, y=5, width=1, height=1}
	guy = Object:new{x=4, y=5, anim_set="ring_bearer", person=true, move_type=MOVE_WANDER}
end

function stop()
end

function update(step)
	down:update()

	if (down.go_time) then
		change_areas("castle_tower4_1", 1, 5, DIRECTION_EAST)
	end

	guy:move(step)
end

function activate(activator, activated)
	if (activated == guy.id) then
		setObjectDirection(guy.id, player_dir(guy))
		smartDialogue{"I'm from Flowey but I'm not much of a mystic...\n",
			MS_BEACH_BATTLE_DONE, "Congratulations on defending Flowey!\n"}
		if (not getMilestone(MS_GOT_RING) and getMilestone(MS_GOT_BADGE)) then
			doDialogue("That's a nice badge! It's a badge of the old Flowey guard... my grandfather was one of them!\nWant to trade?\n", true)
			slot = findUnfullInventorySlot(ITEM_BADGE)
			setInventory(slot, ITEM_RING, 1)
			loadPlayDestroy("chest.ogg")
			setMilestone(MS_GOT_RING, true)
			doDialogue(_t("Got a ") .. getItemIcon(ITEM_RING) .. _t("Silver") .. "\n", true)
		end
	end
end

function collide(id1, id2)
end

