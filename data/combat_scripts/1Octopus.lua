num_enemies = 1
boss_fight = false
experience=215
gold=215

function start()
	addEnemy("Octopus", 60, 105)
end

function get_speech()
	return nil
end

function get_item()
	return ITEM_CURE2
end

