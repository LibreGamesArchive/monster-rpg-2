hp = 500
attack = 75
defense = 60
speed = 10
mdefense = 40
luck = 0

strength = ELEMENT_NONE
weakness = ELEMENT_NONE

function start()
end

function get_action()
	n = getRandomNumber(3)
	if (n == 0) then
		return COMBAT_ATTACKING, 1, getRandomPlayer()
	elseif (n == 1) then
		return COMBAT_CASTING, "Stone", 1, getRandomPlayer()
	else
		return COMBAT_DEFENDING
	end
end

function die()
end

