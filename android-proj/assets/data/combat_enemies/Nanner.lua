hp = 1000
attack = 275
defense = 200
speed = 35
mdefense = 200
luck = 40
float = 0

strength = ELEMENT_NONE
weakness = ELEMENT_NONE

function initId(id)
	myId = id
end

function start()
end

function get_action(step)
	return COMBAT_ATTACKING, 1, getRandomPlayer()
end

function die()
end

