num_enemies = 3
boss_fight = false 
experience=100
gold=100

function start()
	addEnemy("Meatball", 25, 60)
	addEnemy("Meatball", 70, 80)
	addEnemy("Meatball", 30, 100)
end

function get_speech()
	return nil
end

function get_item()
	n = getRandomNumber(20)
	if (n < 5) then
		return ITEM_HEAL
	elseif (n == 10) then
		return ITEM_CURE
	else
		return -1
	end
end

