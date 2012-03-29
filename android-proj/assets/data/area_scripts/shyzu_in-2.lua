music = "shyzu.caf"

function start()
	out = Portal:new{x=2, y=6}
	shopkeep = Object:new{x=2, y=2, anim_set="shyzu_female", person=true}
	setObjectDirection(shopkeep.id, DIRECTION_SOUTH)
	shop = Object:new{x=2, y=3}
end

function stop()
end

function update(step)
	if (out:update()) then
		change_areas("shyzu", 18, 10, DIRECTION_SOUTH)
	end
end

function activate_shop()
	doDialogue("Shopkeep: I just finished making some spicy meatballs...\nShopkeep: Warning, vegetarians may not be able to keep the meatballs down!\n", true)
	doShop("Shopkeep", "shyzu_shopkeep", 5, ITEM_CURE3, 50, ITEM_HEAL, 100, ITEM_ELIXIR, 400, ITEM_HOLY_WATER, 200, ITEM_MEATBALLS, 1000)
end

function activate(activator, activated)
	if (activated == shop.id) then
		activate_shop()
	end
end

function activate_any(tx, ty)
	local px, py = getObjectPosition(0)
	if (px == 2 and py == 4 and tx == 2 and (ty == 2)) then
		setObjectDirection(0, DIRECTION_NORTH);
		activate_shop()
		return true
	end
	return false
end

function collide(id1, id2)
end

