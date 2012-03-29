hp = 1100
attack = 280
defense = 230
speed = 55
mdefense = 600
luck = 60 
float = 0

strength = ELEMENT_NONE
weakness = ELEMENT_NONE

function initId(id)
	myId = id
end

function start()
end

function get_action(step)
	return COMBAT_CASTING, "Banana", 1, getRandomPlayer()
end

function die()
end

