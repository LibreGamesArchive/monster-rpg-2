num_enemies = 1
boss_fight = false
experience=36
gold = 30

function start()
	addEnemy("Hornet", 36, 85)
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

