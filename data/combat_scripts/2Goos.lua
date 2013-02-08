num_enemies = 2
boss_fight = false 
experience=24
gold=20

function start()
	addEnemy("Goo", 22, 70)
	addEnemy("Goo", 52, 90)
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

