num_enemies = 3
boss_fight = false 
experience=140
gold=110

function start()
	addEnemy("Flux", 45, 105)
	addEnemy("Flux", 75, 60)
	addEnemy("Fungus", 60, 70)
end

function get_speech()
	return nil
end

function get_item()
	if (getRandomNumber(5) == 0) then
		return ITEM_HEAL
	else
		return -1
	end
end

