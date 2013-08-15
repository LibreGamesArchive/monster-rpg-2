num_enemies = 3
boss_fight = true 
experience=250
gold=175

function start()
	addEnemy("Stomper", 55, 92)
	addEnemy("Imp", 110, 60)
	addEnemy("Imp", 110, 100)
end

function get_speech()
	return nil
end

function get_item()
	return ITEM_HOLY_WATER
end

