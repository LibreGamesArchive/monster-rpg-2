num_enemies = 3
boss_fight = false 
experience=480
gold=200

function start()
	addEnemy("Grinner", 25, 60)
	addEnemy("Grinner", 70, 80)
	addEnemy("Grinner", 30, 100)
end

function get_speech()
	return nil
end

function get_item()
	return ITEM_HOLY_WATER
end

