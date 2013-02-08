hp = 120
attack = 55
defense = 40
speed = 34
mdefense = 0
luck = 8
float = 0

strength = ELEMENT_NONE
weakness = ELEMENT_NONE

function start()
end

function get_action()
	return COMBAT_ATTACKING, 1, getRandomPlayer()
end

function die()
end

