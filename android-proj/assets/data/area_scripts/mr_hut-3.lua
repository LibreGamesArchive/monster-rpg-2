music = "Muttrace.caf"

function start()
	out = Portal:new{x=3, y=7}
	shopkeep = Object:new{x=3, y=2, anim_set="Doggy", person=true}
	setObjectDirection(shopkeep.id, DIRECTION_SOUTH)
	shop = Object:new{x=3, y=3}
end

function stop()
end

function update(step)
	if (out:update()) then
		change_areas("Muttrace", 20, 8, DIRECTION_SOUTH)
	end
end

function activate_shop()
	doDialogue("Shopkeeper: Everything is on sale for you my friend.\n", true)
	doShop("Shopkeep", "Muttrace_shopkeeper", 5, ITEM_LIGHT_SWORD, 500, ITEM_LIGHT_STAFF, 500, ITEM_LIGHT_HELMET, 500, ITEM_LIGHT_ARMOR, 500, ITEM_LIGHT_BOOTS, 500)
end

function activate(activator, activated)
	if (activated == shop.id) then
		activate_shop()
	end
end

function activate_any(tx, ty)
	local px, py = getObjectPosition(0)
	if (px == 3 and py == 4 and tx == 3 and ty == 2) then
		setObjectDirection(0, DIRECTION_NORTH);
		activate_shop()
		return true
	end
	return false
end

function collide(id1, id2)
end

