num_enemies = 4
boss_fight = false 
experience=100
gold=100

function start()
	addEnemy("Slime", 30, 60)
	addEnemy("Larva", 60, 80)
	addEnemy("Slime", 30, 105)
	addEnemy("Larva", 75, 100)
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

