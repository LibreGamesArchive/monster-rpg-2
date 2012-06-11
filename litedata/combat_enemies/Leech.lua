hp = 20
attack = 15
defense = 10
speed = 5
mdefense = 0
luck = 20 

strength = ELEMENT_NONE
weakness = ELEMENT_NONE

function start()
end

function get_action()
	return COMBAT_ATTACKING, 1, getRandomPlayer()
end

function die()
end

