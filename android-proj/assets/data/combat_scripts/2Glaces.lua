num_enemies = 2
boss_fight = false 
experience=380
gold=250

function start()
	addEnemy("Glace", 32, 92)
	addEnemy("Glace", 72, 100)
end

function get_speech()
	return nil
end

function get_item()
	if (getRandomNumber(3) == 0) then
		return ITEM_ELIXIR
	else
		return -1
	end
end

