num_enemies = 1
boss_fight = false
experience=300
gold=200
can_run = false

function start()
	addEnemy("Troll", 60, 105)
end

function get_speech()
	return nil
end

function get_item()
	return ITEM_ELIXIR
end

