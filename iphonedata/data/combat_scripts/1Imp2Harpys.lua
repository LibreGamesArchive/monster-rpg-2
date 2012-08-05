num_enemies = 3
boss_fight = false 
experience=160
gold=160

function start()
	addEnemy("Imp", 30, 60)
	addEnemy("Harpy", 50, 80)
	addEnemy("Harpy", 70, 100)
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

