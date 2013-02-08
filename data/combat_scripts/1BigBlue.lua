num_enemies = 1
boss_fight = false 
experience=310
gold=150

function start()
	addEnemy("BigBlue", 50, 105)
end

function get_speech()
	return nil
end

function get_item()
	return ITEM_ELIXIR
end

