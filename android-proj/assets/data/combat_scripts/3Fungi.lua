num_enemies = 3
boss_fight = false 
experience=170
gold=170

function start()
	addEnemy("Fungus", 42, 70)
	addEnemy("Fungus", 62, 85)
	addEnemy("Fungus", 42, 100)
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

