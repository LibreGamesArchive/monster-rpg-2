num_enemies = 5
boss_fight = false 
experience=330
gold=200

function start()
	addEnemy("Husk", 29, 70);
	addEnemy("Husk", 29, 90);
	addEnemy("Husk", 58, 74);
	addEnemy("Husk", 58, 94);
	addEnemy("Husk", 87, 84);
end

function get_speech()
	return nil
end

function get_item()
	local n = getRandomNumber(3)
	if (n == 0) then
		return ITEM_HEAL
	else
		return -1
	end
end

