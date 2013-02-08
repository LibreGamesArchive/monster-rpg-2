num_enemies = 2
boss_fight = false 
experience=195
gold=130

function start()
	addEnemy("Creep", 42, 92)
	addEnemy("Creep", 72, 97)
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

