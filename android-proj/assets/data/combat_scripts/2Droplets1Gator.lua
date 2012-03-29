num_enemies = 3
boss_fight = false 
experience=250
gold=250

function start()
	addEnemy("Droplet", 85, 80)
	addEnemy("Droplet", 95, 100)
	addEnemy("Gator", 40, 105)
end

function get_speech()
	return nil
end

function get_item()
	if (getRandomNumber(5) == 0) then
		return ITEM_ELIXIR
	else
		return -1
	end
end

