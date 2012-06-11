num_enemies = 2
boss_fight = false 
experience=90
gold=90

function start()
	addEnemy("Chomper", 30, 85)
	addEnemy("Chomper", 60, 105)
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

