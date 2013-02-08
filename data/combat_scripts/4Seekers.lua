num_enemies = 4
boss_fight = false 
experience=450
gold=100

function start()
	addEnemy("Seeker", 35, 65)
	addEnemy("Seeker", 90, 75)
	addEnemy("Seeker", 40, 100)
	addEnemy("Seeker", 95, 105)
end

function get_speech()
	return nil
end

function get_item()
	n = getRandomNumber(3)
	if (n == 0) then
		return ITEM_HOLY_WATER
	else
		return -1
	end
end

