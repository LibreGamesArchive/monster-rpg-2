num_enemies = 3
boss_fight = false 
experience=15
gold=10

function start()
	addEnemy("Leech", 25, 60)
	addEnemy("Leech", 60, 80)
	addEnemy("Leech", 25, 100)
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

