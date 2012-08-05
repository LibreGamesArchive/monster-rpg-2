num_enemies = 3
boss_fight = false 
experience=75
gold=100

function start()
	addEnemy("Coyote", 30, 60)
	addEnemy("Coyote", 50, 80)
	addEnemy("Coyote", 70, 100)
end

function get_speech()
	return nil
end

function get_item()
	if (getRandomNumber(20) == 0) then
		return ITEM_CURE
	elseif (getRandomNumber(20) == 0) then
		return ITEM_HEAL
	else
		return -1
	end
end

