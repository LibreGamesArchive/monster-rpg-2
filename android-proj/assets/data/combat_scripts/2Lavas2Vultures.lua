num_enemies = 4
boss_fight = false 
experience=310
gold=200

function start()
	addEnemy("Lava", 32, 92)
	addEnemy("Lava", 60, 105)
	addEnemy("Vulture", 105, 85)
	addEnemy("Vulture", 113, 100)
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

