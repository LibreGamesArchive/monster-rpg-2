hp = 2000
attack = 400
defense = 400
speed = 70 
mdefense = 300
luck = 70
float = 0

strength = ELEMENT_FIRE
weakness = ELEMENT_ICE

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
