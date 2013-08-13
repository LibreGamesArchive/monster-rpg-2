num_enemies = 3
boss_fight = true 
experience=200
gold=150

function start()
	addEnemy("Witch", 30, 80)
	addEnemy("Cub", 60, 60)
	addEnemy("Cub", 60, 100)
end

function get_speech()
	return nil
end

function get_item()
	return ITEM_HOLY_WATER
end

