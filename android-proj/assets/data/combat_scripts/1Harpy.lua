num_enemies = 1
boss_fight = false
experience=100
gold=100

function start()
	addEnemy("Harpy", 45, 90)
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

