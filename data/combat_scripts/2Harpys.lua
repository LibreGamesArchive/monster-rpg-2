num_enemies = 2
boss_fight = false 
experience=120
gold=100

function start()
	addEnemy("Harpy", 42, 80)
	addEnemy("Harpy", 62, 100)
end

function get_speech()
	return nil
end

function get_item()
	if (getRandomNumber(20) == 0) then
		return ITEM_CURE2
	else
		return -1
	end
end

