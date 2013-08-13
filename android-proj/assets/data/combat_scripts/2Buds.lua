num_enemies = 2
boss_fight = false 
experience=50
gold=55

function start()
	addEnemy("Bud", 32, 80)
	addEnemy("Bud", 62, 100)
end

function get_speech()
	return nil
end

function get_item()
	if (getRandomNumber(20) == 0) then
		return ITEM_CURE
	else
		return -1
	end
end

