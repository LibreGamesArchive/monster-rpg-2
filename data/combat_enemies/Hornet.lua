hp = 60
attack = 25
defense = 10
speed = 30
mdefense = 0
luck = 5
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

