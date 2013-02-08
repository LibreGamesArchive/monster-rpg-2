num_enemies = 1
boss_fight = true
experience=300
gold=200

function start()
	addEnemy("Possessed", 55, 105)
end

function get_speech()
	return nil
end

function get_item()
	return ITEM_CURE3
end

