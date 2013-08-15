music = "happtroll.ogg"

function start()
	shopPortal = Portal:new{x=4, y=9}
	shopkeeper = Object:new{x=4, y=3, anim_set="Seaside_shopkeeper"}
	shop = Object:new{x=4, y=5}
end

function stop()
end

function update(step)
	shopPortal:update()

	if (shopPortal.go_time) then
		if (getMilestone(MS_BEAT_TODE)) then
			change_areas("seaside_repaired", 35, 8, DIRECTION_SOUTH);
		else
			change_areas("seaside", 35, 8, DIRECTION_SOUTH);
		end
	end
end

function activate_shop()
	if (not getMilestone(MS_BEAT_TODE)) then
		doDialogue("Shopkeeper: Luckily for me, when the fires started, I had a rain spell!\n", true)
	end
	doShop("Shopkeeper", "Seaside_shopkeeper", 7, ITEM_CURE, 10, ITEM_HEAL, 25, ITEM_ELIXIR, 225, ITEM_HOLY_WATER, 200, ITEM_IRON_STAFF, 75, ITEM_CLOTH_ROBE, 50, ITEM_SANDALS, 50)
end

function activate(activator, activated)
	if (activated == shop.id) then
		activate_shop()
	end
end

function activate_any(tx, ty)
	local px, py = getObjectPosition(0)
	if (px == 4 and py == 6 and tx == 4 and (ty == 4 or ty == 3)) then
		setObjectDirection(0, DIRECTION_NORTH);
		activate_shop()
		return true
	end
	return false
end

function collide(id1, id2)
end

