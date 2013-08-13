hp = 1000
attack = 400
defense = 400
speed = 80
mdefense = 500 
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

function get_attack_condition()
	return CONDITION_POISONED
end
