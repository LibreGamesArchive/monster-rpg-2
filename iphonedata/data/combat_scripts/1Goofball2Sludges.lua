num_enemies = 3
boss_fight = false 
experience=140
gold=140

function start()
	addEnemy("Goofball", 30, 60)
	addEnemy("Sludge", 50, 80)
	addEnemy("Sludge", 70, 100)
end

function get_speech()
	return nil
end

function get_item()
	if (getRandomNumber(20) == 0) then
		return ITEM_CURE2
	elseif (getRandomNumber(20) == 0) then
		return ITEM_HEAL
	else
		return -1
	end
end

