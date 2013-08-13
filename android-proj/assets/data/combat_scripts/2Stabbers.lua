num_enemies = 2
boss_fight = false 
experience=100
gold=100
can_run = false

function start()
	addEnemy("Stabber", 32, 80)
	addEnemy("Stabber", 62, 100)
end

function get_speech()
	return nil
end

function get_item()
	if (getRandomNumber(20) == 0) then
		return ITEM_CURE2
	else
		return -1
	end
end

