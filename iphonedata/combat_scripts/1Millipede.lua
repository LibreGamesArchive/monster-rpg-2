num_enemies = 1
boss_fight = false
experience=340
gold=200

function start()
	addEnemy("Millipede", 50, 100)
end

function get_speech()
	return nil
end

function get_item()
	if (getRandomNumber(5) == 0) then
		return ITEM_CURE3
	else
		return -1
	end
end

