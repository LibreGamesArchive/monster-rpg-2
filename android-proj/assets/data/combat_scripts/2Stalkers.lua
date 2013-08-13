num_enemies = 2
boss_fight = false 
experience=190
gold=120

function start()
	addEnemy("Stalker", 42, 80)
	addEnemy("Stalker", 62, 100)
end

function get_speech()
	return nil
end

function get_item()
	if (getRandomNumber(20) == 0) then
		return ITEM_CURE3
	else
		return -1
	end
end

