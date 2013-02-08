num_enemies = 2
boss_fight = false 
experience=150
gold=150

function start()
	addEnemy("Toad", 42, 80)
	addEnemy("Toad", 62, 100)
end

function get_speech()
	return nil
end

function get_item()
	if (getRandomNumber(10) == 0) then
		return ITEM_HEAL
	else
		return -1
	end
end

