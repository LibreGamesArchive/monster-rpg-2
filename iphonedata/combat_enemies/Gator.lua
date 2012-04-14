hp = 1000 
attack = 120
defense = 120
speed = 40
mdefense = 100
luck = 40

strength = ELEMENT_ELECTRICITY
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

