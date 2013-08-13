num_enemies = 3
boss_fight = false 
experience=135
gold=135

function start()
	addEnemy("Goofball", 30, 60)
	addEnemy("Goofball", 50, 80)
	addEnemy("Goofball", 70, 100)
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

