hp = 3000
attack = 700
defense = 400
speed = 100
mdefense = 550
luck = 75
explodes = true

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
