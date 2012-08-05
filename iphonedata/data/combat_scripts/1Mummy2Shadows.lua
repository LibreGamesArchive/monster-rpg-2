num_enemies = 3
boss_fight = false 
experience=300
gold=200

function start()
	addEnemy("Shadow", 85, 95)
	addEnemy("Shadow", 95, 65)
	addEnemy("Mummy", 40, 90)
end

function get_speech()
	return nil
end

function get_item()
	return ITEM_ELIXIR
end

