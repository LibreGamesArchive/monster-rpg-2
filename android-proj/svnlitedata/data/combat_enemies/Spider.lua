hp = 100
attack = 22
defense = 12
speed = 25
mdefense = 0
luck = 10

strength = ELEMENT_NONE
weakness = ELEMENT_NONE

function start()
end

function get_action()
	return COMBAT_ATTACKING, 1, getRandomPlayer()
end

function die()
end

