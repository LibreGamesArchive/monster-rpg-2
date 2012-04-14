num_enemies = 3
boss_fight = false 
experience=360
gold=200

function start()
	addEnemy("Flamer", 40, 85)
	addEnemy("Magmer", 90, 85);
	addEnemy("Magmer", 100, 105);
end

function get_speech()
	return nil
end

function get_item()
	if (getRandomNumber(20) == 0) then
		return ITEM_CURE3
	else
		return -1
	end
end

