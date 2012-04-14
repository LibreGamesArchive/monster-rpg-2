hp = 80
attack = 20
defense = 10
speed = 20
mdefense = 0
luck = 0
float = 0

strength = ELEMENT_NONE
weakness = ELEMENT_NONE

function start()
end

function get_action()
	n = getRandomNumber(2)
	if (n == 0) then
		return COMBAT_ATTACKING, 1, getRandomPlayer()
	else
		return COMBAT_CASTING, "Darkness1", -1, nil
	end
end

function die()
end

