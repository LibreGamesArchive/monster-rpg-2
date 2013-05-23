num_enemies = 3
boss_fight = false 
experience=355
gold=160

function start()
	addEnemy("BigCheese", 40, 105)
	addEnemy("BigCheese", 50, 75)
	addEnemy("BigCheese", 90, 90)
end

function get_speech()
	return nil
end

function get_item()
	return ITEM_HEAL
end

