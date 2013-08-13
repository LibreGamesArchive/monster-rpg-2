num_enemies = 3
boss_fight = false 
experience=360
gold=200

function start()
	addEnemy("Kromagma", 40, 85)
	addEnemy("Flamer", 100, 90);
	addEnemy("Zombie", 75, 105);
end

function get_speech()
	return nil
end

function get_item()
	return ITEM_CURE3
end

