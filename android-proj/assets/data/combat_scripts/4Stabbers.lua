num_enemies = 4
boss_fight = false 
experience=100
gold=100

function start()
	addEnemy("Stabber", 25, 70)
	addEnemy("Stabber", 70, 80)
	addEnemy("Stabber", 30, 90)
	addEnemy("Stabber", 75, 95)
end

function get_speech()
	return nil
end

function get_item()
	n = getRandomNumber(5)
	if (n == 0) then
		return ITEM_CURE2
	else
		return -1
	end
end

