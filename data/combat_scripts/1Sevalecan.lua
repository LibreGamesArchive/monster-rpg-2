num_enemies = 1
boss_fight = false 
experience=330
gold=150

function start()
	addEnemy("Sevalecan", 50, 100)
end

function get_speech()
	return nil
end

function get_item()
	return ITEM_ELIXIR
end

