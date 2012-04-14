num_enemies = 1
boss_fight = false
experience=85
gold=85

function start()
	addEnemy("Imp", 50, 95)
end

function get_speech()
	return nil
end

function get_item()
	if (randint(20) < 2) then
		return ITEM_CURE2
	end

	return -1
end

