hp = 800 
attack = 225
defense = 190
speed = 30
mdefense = 180
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

