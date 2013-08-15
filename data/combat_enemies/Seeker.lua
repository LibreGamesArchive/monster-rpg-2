hp = 500
attack = 550
defense = 400
speed = 80
mdefense = 400
luck = 120 

strength = ELEMENT_NONE
weakness = ELEMENT_NONE

function initId(id)
	myId = id
end

function start()
end

function get_action(step)
	return COMBAT_ATTACKING, 1, getWeakestPlayer()
end

function die()
end
