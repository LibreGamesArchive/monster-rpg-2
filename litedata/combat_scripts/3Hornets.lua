num_enemies = 3
boss_fight = false 
experience=60
gold=60

function start()
	addEnemy("Hornet", 30, 61)
	addEnemy("Hornet", 50, 81)
	addEnemy("Hornet", 60, 101)
end

function get_speech()
	return nil
end

function get_item()
	if (getRandomNumber(20) == 0) then
		return ITEM_CURE
	elseif (getRandomNumber(20) == 0) then
		return ITEM_HEAL
	else
		return -1
	end
end

