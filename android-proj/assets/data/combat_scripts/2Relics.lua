num_enemies = 2
boss_fight = false 
experience=400
gold=500

function start()
	addEnemy("Relic", 50, 70);
	addEnemy("Relic", 70, 100);
end

function get_speech()
	return nil
end

function get_item()
	if (getRandomNumber(2) == 0) then
		return ITEM_HOLY_WATER
	end
	return -1
end

