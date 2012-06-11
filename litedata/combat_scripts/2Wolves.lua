num_enemies = 2
boss_fight = false 
experience=50
gold=80

function start()
	addEnemy("Wolf", 32, 70)
	addEnemy("Wolf", 62, 90)
end

function get_speech()
	return nil
end

function get_item()
	if (getRandomNumber(20) == 0) then
		return ITEM_CURE
	else
		return -1
	end
end

