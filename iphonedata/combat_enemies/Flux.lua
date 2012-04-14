hp = 300 
attack = 120
defense = 130
speed = 33
mdefense = 110
luck = 35

strength = ELEMENT_ELECTRICITY
weakness = ELEMENT_NONE

function initId(id)
	myId = id
end

function start()
end

function get_action(step)
	return COMBAT_CASTING, "Acid", 1, getRandomPlayer()
end

function die()
end

