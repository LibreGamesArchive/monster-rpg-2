num_enemies = 2
boss_fight = false 
experience=450
gold=100

function start()
	addEnemy("Evil", 50, 60);
	addEnemy("Evil", 70, 90);
end

function get_speech()
	return nil
end

function get_item()
	if (getRandomNumber(2) == 0) then
		return ITEM_ELIXIR
	end
	return -1
end

