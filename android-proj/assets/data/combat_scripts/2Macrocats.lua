num_enemies = 2
boss_fight = false 
experience=360
gold=250

function start()
	addEnemy("Macrocat", 50, 60);
	addEnemy("Macrocat", 70, 100);
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

