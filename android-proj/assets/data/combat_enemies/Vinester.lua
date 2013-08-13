hp = 750
attack = 300 
defense = 250
speed = 85
mdefense = 300
luck = 100 

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

