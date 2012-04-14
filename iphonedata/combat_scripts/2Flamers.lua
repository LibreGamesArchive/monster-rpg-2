num_enemies = 2
boss_fight = false 
experience=300
gold=180

function start()
	addEnemy("Flamer", 40, 85)
	addEnemy("Flamer", 80, 100)
end

function get_speech()
	return nil
end

function get_item()
	if (getRandomNumber(20) == 0) then
		return ITEM_CURE3
	else
		return -1
	end
end

