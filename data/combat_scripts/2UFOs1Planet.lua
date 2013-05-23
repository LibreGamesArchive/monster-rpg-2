num_enemies = 3
boss_fight = false 
experience=380
gold=300

function start()
	addEnemy("UFO", 30, 70);
	addEnemy("UFO", 40, 90);
	addEnemy("Planet", 90, 80);
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

