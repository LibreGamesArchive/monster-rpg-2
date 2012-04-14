num_enemies = 1
boss_fight = false
experience=600
gold=250
--can_run = false

function start()
	addEnemy("DarkMan", 60, 95)
end

function get_speech()
	return nil
end

function get_item()
	local n = getRandomNumber(2)
	if (n == 0) then
		return ITEM_CURE3
	else
		return -1
	end
end

