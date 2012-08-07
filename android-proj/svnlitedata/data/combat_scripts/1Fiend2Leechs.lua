num_enemies = 3
boss_fight = false 
experience=45
gold=40

function start()
	addEnemy("Leech", 65, 60)
	addEnemy("Fiend", 35, 80)
	addEnemy("Leech", 65, 100)
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

