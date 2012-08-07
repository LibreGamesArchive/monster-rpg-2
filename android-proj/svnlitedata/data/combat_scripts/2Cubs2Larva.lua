num_enemies = 4
boss_fight = false 
experience=80
gold=80

function start()
	addEnemy("Cub", 30, 60)
	addEnemy("Larva", 30, 105)
	addEnemy("Cub", 75, 105)
	addEnemy("Larva", 75, 60)
end

function get_speech()
	return nil
end

function get_item()
	if (getRandomNumber(5) == 0) then
		return ITEM_CURE2
	else
		return -1
	end
end

