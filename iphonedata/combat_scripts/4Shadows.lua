num_enemies = 4
boss_fight = false 
experience=400
gold=200

function start()
	addEnemy("Shadow", 30, 70);
	addEnemy("Shadow", 40, 90);
	addEnemy("Shadow", 80, 70);
	addEnemy("Shadow", 90, 90);
end

function get_speech()
	return nil
end

function get_item()
	return ITEM_CURE3
end

