hp = 65
attack = 35
defense = 15
speed = 30
mdefense = 0
luck = 15

strength = ELEMENT_NONE
weakness = ELEMENT_FIRE

function start()
end

function get_action()
	return COMBAT_ATTACKING, 1, getRandomPlayer()
end

function die()
end

