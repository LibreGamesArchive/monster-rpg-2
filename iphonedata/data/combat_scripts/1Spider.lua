num_enemies = 1
boss_fight = false 
experience=45
gold=50

function start()
	addEnemy("Spider", 50, 95)
end

function get_speech()
	return nil
end

function get_item()
	n = getRandomNumber(20)
	if (n == 0) then
		return ITEM_CURE
	elseif (n == 1) then
		return ITEM_HEAL
	else
		return -1
	end
end

