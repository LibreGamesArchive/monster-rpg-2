num_enemies = 2
boss_fight = false 
experience=80
gold=70

function start()
	addEnemy("Meatball", 65, 80)
	addEnemy("Wasp", 35, 100)
end

function get_speech()
	return nil
end

function get_item()
	n = getRandomNumber(20)
	if (n == 0) then
		return ITEM_CURE2
	elseif (n == 1) then
		return ITEM_HEAL
	else
		return -1
	end
end

