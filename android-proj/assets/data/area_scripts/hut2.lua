music = "jungle_shop.caf"

function start()
	out = Portal:new{x=3, y=7}

	shopkeep = Object:new{x=3, y=3, anim_set="villager5", person=true}
	setObjectDirection(shopkeep.id, DIRECTION_SOUTH)
	shop = Object:new{x=3, y=4}
end

function stop()
end

function update(step)
	if (out:update()) then
		change_areas("jungle_village", 23, 14, DIRECTION_SOUTH);
	end
end

function activate_shop()
	doShop("Shopkeeper", "jungle_shopkeep", 10, ITEM_CURE3, 50, ITEM_HEAL, 100, ITEM_ELIXIR, 400, ITEM_HOLY_WATER, 200, ITEM_ONYX_SWORD, 1000, ITEM_ONYX_STAFF, 750, ITEM_ONYX_CAP, 500, ITEM_ONYX_ARMOR, 700, ITEM_ONYX_ROBE, 600, ITEM_ONYX_SHOES, 500)
end

function activate(activator, activated)
	if (activated == shop.id) then
		activate_shop()
	end
end

function activate_any(tx, ty)
	local px, py = getObjectPosition(0)
	if (px == 3 and py == 5 and tx == 3 and ty == 3) then
		setObjectDirection(0, DIRECTION_NORTH);
		activate_shop()
		return true
	end
	return false
end

function collide(id1, id2)
end

