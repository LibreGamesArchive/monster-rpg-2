num_enemies = 4
boss_fight = false 
experience=310
gold=200

function start()
	addEnemy("Zombie", 30, 70);
	addEnemy("Zombie", 40, 90);
	addEnemy("Zombie", 80, 70);
	addEnemy("Zombie", 90, 90);
end

function get_speech()
	return nil
end

function get_item()
	return ITEM_CURE3
end

