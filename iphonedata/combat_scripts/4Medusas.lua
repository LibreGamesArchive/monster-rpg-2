num_enemies = 4
boss_fight = false 
experience=100
gold=100

function start()
	addEnemy("Medusa", 25, 60)
	addEnemy("Medusa", 80, 70)
	addEnemy("Medusa", 30, 100)
	addEnemy("Medusa", 85, 105)
end

function get_speech()
	return nil
end

function get_item()
	n = getRandomNumber(5)
	if (n == 0) then
		return ITEM_HEAL
	else
		return -1
	end
end

