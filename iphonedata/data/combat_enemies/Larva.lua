hp = 100
attack = 40
defense = 20
speed = 5
mdefense = 0
luck = 30 

strength = ELEMENT_NONE
weakness = ELEMENT_NONE

function start()
end

function get_action()
	return COMBAT_ATTACKING, 1, getRandomPlayer()
end

function die()
end

