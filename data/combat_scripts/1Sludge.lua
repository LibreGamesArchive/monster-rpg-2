num_enemies = 1
boss_fight = false
experience=100
gold=100

function start()
	addEnemy("Sludge", 50, 95)
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

