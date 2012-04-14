hp = 800
attack = 45
defense = 10
speed = 20
mdefense = 0
luck = 12

strength = ELEMENT_NONE
weakness = ELEMENT_ELECTRICITY

function start()
end

function get_action()
	n = getRandomNumber(5)

	if (n == 0 or n == 1) then
		return COMBAT_CASTING, "Wave", -1, nil
	elseif (n == 2 or n == 3) then
		return COMBAT_CASTING, "Whirlpool", 1, getRandomPlayer()
	else
		return COMBAT_ATTACKING, 1, getRandomPlayer()
	end
end

function die()
end

