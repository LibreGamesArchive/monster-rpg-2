num_enemies = 3
boss_fight = false 
experience=75
gold=100

function start()
	addEnemy("Rage", 50, 85)
	addEnemy("Rage", 80, 95)
	addEnemy("Rage", 50, 105)
end

function get_speech()
	return nil
end

function get_item()
	if (getRandomNumber(20) == 0) then
		return ITEM_CURE2
	elseif (getRandomNumber(20) == 0) then
		return ITEM_HEAL
	else
		return -1
	end
end

