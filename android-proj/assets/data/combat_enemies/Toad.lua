hp = 500 
attack = 150
defense = 180
speed = 33
mdefense = 130
luck = 15

strength = ELEMENT_NONE
weakness = ELEMENT_ELECTRICITY

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
	n = getRandomNumber(2);
	if (n == 0) then
		return CONDITION_POISONED
	else
		return CONDITION_NORMAL
	end
end
