hp = 2500
attack = 375
defense = 9999
speed = 50
mdefense = 9999
luck = 50

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
