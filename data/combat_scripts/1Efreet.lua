num_enemies = 1
boss_fight = false 
experience=310
gold=150

function start()
	addEnemy("Efreet", 40, 100)
end

function get_speech()
	return nil
end

function get_item()
	if (randint(2) == 0) then
		return ITEM_ELIXIR
	else
		return -1
	end
end

