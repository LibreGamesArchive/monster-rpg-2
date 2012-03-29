hp = 5000
attack = 550
defense = 500
speed = 50
mdefense = 500
luck = 80

strength = ELEMENT_NONE
weakness = ELEMENT_FIRE

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
