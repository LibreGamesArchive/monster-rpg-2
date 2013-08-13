num_enemies = 1
boss_fight = true 
experience=100
gold=100

function start()
	addEnemy("Golem", 50, 95)
end

function get_speech()
	return nil
end

function get_item()
	return ITEM_HEAL
end

