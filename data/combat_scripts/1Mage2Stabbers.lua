num_enemies = 3
boss_fight = false 
experience=150
gold=150

function start()
	addEnemy("Mage", 30, 85)
	addEnemy("Stabber", 75, 70)
	addEnemy("Stabber", 75, 100)
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

