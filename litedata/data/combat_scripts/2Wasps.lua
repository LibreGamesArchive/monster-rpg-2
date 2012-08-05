num_enemies = 2
boss_fight = false 
experience=80
gold=80

function start()
	addEnemy("Wasp", 30, 65)
	addEnemy("Wasp", 60, 95)
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

