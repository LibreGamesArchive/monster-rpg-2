hp = 5
attack = 60
defense = 1000
speed = 10
mdefense = -50
luck = 26 

strength = ELEMENT_NONE
weakness = ELEMENT_NONE

function start()
end

function get_action()
	return COMBAT_ATTACKING, 1, getRandomPlayer()
end

function die()
end

