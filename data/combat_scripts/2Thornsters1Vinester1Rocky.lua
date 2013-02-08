num_enemies = 4
boss_fight = false 
experience=240
gold=200

function start()
	addEnemy("Thornster", 35, 65)
	addEnemy("Thornster", 35, 100)
	addEnemy("Vinester", 75, 65)
	addEnemy("Rocky", 75, 100)
end

function get_speech()
	return nil
end

function get_item()
	return ITEM_CURE2
end

