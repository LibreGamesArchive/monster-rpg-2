num_enemies = 4
boss_fight = false 
experience=330
gold=200

function start()
	addEnemy("UFO", 30, 70);
	addEnemy("UFO", 40, 90);
	addEnemy("UFO", 80, 70);
	addEnemy("UFO", 90, 90);
end

function get_speech()
	return nil
end

function get_item()
	return ITEM_CURE3
end

