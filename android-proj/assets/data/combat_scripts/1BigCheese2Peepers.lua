num_enemies = 3
boss_fight = false 
experience=340
gold=200

function start()
	addEnemy("Peeper", 85, 105)
	addEnemy("Peeper", 95, 75)
	addEnemy("BigCheese", 40, 90)
end

function get_speech()
	return nil
end

function get_item()
	if (getRandomNumber(5) == 0) then
		return ITEM_CURE3
	end
	return -1
end

