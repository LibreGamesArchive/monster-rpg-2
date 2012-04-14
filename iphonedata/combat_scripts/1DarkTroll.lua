num_enemies = 1
boss_fight = false
experience=200
gold=200

function start()
	addEnemy("DarkTroll", 60, 105)
end

function get_speech()
	return nil
end

function get_item()
	local n = getRandomNumber(5)
	if (n == 0) then
		return ITEM_CURE2
	else
		return -1
	end
end

