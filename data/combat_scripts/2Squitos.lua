num_enemies = 2
boss_fight = false 
experience=185
gold=120

function start()
	addEnemy("Squito", 42, 92)
	addEnemy("Squito", 72, 97)
end

function get_speech()
	return nil
end

function get_item()
	if (getRandomNumber(20) == 0) then
		return ITEM_CURE3
	else
		return -1
	end
end

