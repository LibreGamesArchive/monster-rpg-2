num_enemies = 3
boss_fight = false 
experience=400
gold=300

function start()
	addEnemy("Awoken", 30, 80);
	addEnemy("Awoken", 40, 100);
	addEnemy("Relic", 90, 90);
end

function get_speech()
	return nil
end

function get_item()
	if (getRandomNumber(2) == 0) then
		return ITEM_CURE3
	end
	return -1
end

