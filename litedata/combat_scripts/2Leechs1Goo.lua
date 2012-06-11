num_enemies = 3
boss_fight = false 
experience=27
gold=20

function start()
	addEnemy("Leech", 22, 60)
	addEnemy("Goo", 60, 80)
	addEnemy("Leech", 22, 100)
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

