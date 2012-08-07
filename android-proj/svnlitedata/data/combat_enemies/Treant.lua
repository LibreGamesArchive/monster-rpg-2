hp = 100
attack = 70
defense = 15
speed = 10
mdefense = 0
luck = 25

strength = ELEMENT_NONE
weakness = ELEMENT_FIRE

function start()
end

function get_action()
	return COMBAT_ATTACKING, 1, getRandomPlayer()
end

function die()
end

