num_enemies = 2
boss_fight = false 
experience=80
gold=120

function start()
	addEnemy("Treant", 45, 95)
	addEnemy("Treant", 80, 105)
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

