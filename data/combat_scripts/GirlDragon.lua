num_enemies = 1
boss_fight = true
experience=100
gold=100

function start()
	addEnemy("Girl", 66, 110)
end

function get_speech()
	return nil
end

function get_item()
	return ITEM_ELIXIR
end

