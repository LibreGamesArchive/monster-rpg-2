hp = 3500
attack = 850
defense = 550
speed = 100
mdefense = 600
luck = 85

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
