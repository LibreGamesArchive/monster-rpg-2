num_enemies = 3
boss_fight = false 
experience=400
gold=150

function start()
	addEnemy("Devil", 40, 95)
	addEnemy("Devil", 50, 65)
	addEnemy("Devil", 90, 80)
end

function get_speech()
	return nil
end

function get_item()
	if (getRandomNumber(2) == 0) then
		return ITEM_CURE3
	else
		return -1
	end
end

