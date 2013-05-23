num_enemies = 1
boss_fight = false
experience=200
gold=200

function start()
	addEnemy("Gator", 60, 105)
end

function get_speech()
	return nil
end

function get_item()
	return ITEM_CURE2
end

