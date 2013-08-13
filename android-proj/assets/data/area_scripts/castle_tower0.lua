music = "castle.ogg"

function start()
	exit = Portal:new{x=4, y=9, width=1, height=1}
	shopkeeper = Object:new{x=4, y=3, anim_set="castle_shopkeeper"}
	shop = Object:new{x=4, y=4}
end

function stop()
end

function update(step)
	exit:update()

	if (exit.go_time) then
		change_areas("castle", 15, 16, DIRECTION_SOUTH)
	end
end

function activate_shop()
	if (getMilestone(MS_GUNNAR_LEFT)) then
		doDialogue("Shopkeeper: Welcome! We have just got some new stock!\n", true)
		doShop("Shopkeeper", "Castle_shopkeeper", 9, ITEM_CURE2, 50, ITEM_ELIXIR, 200, ITEM_HOLY_WATER, 150, ITEM_SOLDIER_SWORD, 500, ITEM_SOLDIER_HELMET, 400, ITEM_SOLDIER_BOOTS, 400, ITEM_MAGE_STAFF, 300, ITEM_MAGE_ROBE, 300, ITEM_TOPHAT, 200)
	else
		doDialogue("Shopkeeper: Welcome!\n", true)
		doShop("Shopkeeper", "Castle_shopkeeper", 6, ITEM_CURE, 15, ITEM_HEAL, 30, ITEM_ELIXIR, 250, ITEM_HOLY_WATER, 250, ITEM_LEATHER_VEST, 200, ITEM_LEATHER_CAP, 100)
	end
end

function activate(activator, activated)
	if (activated == shop.id) then
		activate_shop()
	end
end

function activate_any(tx, ty)
	local px, py = getObjectPosition(0)
	if (px == 4 and py == 5 and tx == 4 and ty == 3) then
		setObjectDirection(0, DIRECTION_NORTH);
		activate_shop()
		return true
	end
	return false
end

function collide(id1, id2)
end

