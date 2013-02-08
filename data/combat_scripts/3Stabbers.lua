num_enemies = 3
boss_fight = false 
experience=75
gold=100

function start()
	addEnemy("Stabber", 30, 60)
	addEnemy("Stabber", 50, 80)
	addEnemy("Stabber", 70, 100)
end

function get_speech()
	return nil
end

function get_item()
	if (getRandomNumber(20) == 0) then
		return ITEM_CURE2
	elseif (getRandomNumber(20) == 0) then
		return ITEM_CURE3
	else
		return -1
	end
end

