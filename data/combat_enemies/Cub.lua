hp = 80
attack = 50
defense = 50
speed = 35
mdefense = 0
luck = 5

strength = ELEMENT_NONE
weakness = ELEMENT_NONE

function start()
end

function get_action()
	return COMBAT_ATTACKING, 1, getRandomPlayer()
end

function die()
end

