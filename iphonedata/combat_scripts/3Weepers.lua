num_enemies = 3
boss_fight = false 
experience=36
gold=50

function start()
	addEnemy("Weeper", 25, 60)
	addEnemy("Weeper", 70, 80)
	addEnemy("Weeper", 30, 100)
end

function get_speech()
	return nil
end

function get_item()
	n = getRandomNumber(20)
	if (n == 0) then
		return ITEM_CURE
	elseif (n == 1) then
		return ITEM_HEAL
	else
		return -1
	end
end

