num_enemies = 2
boss_fight = false 
experience=260
gold=150

function start()
	addEnemy("Lava", 32, 92)
	addEnemy("Lava", 72, 100)
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

