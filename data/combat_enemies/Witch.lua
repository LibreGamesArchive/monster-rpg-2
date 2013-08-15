hp = 800
attack = 60 
defense = 30
speed = 20
mdefense = 20
luck = 15

strength = ELEMENT_NONE
weakness = ELEMENT_ELECTRICITY

function start()
end

function get_action()
	if (battleGetNumEnemies() == 1) then
		return COMBAT_CASTING, "Fire2", -1, nil
	else
		n = getRandomNumber(2)
		if (n == 0) then
			return COMBAT_CASTING, "Fire1", 1, getRandomPlayer()
		else
			return COMBAT_ATTACKING, 1, getRandomPlayer()
		end
	end
end

function die()
end

