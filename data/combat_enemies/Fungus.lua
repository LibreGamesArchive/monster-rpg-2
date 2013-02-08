hp = 400 
attack = 120
defense = 160
speed = 35
mdefense = 200 
luck = 35

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

function get_attack_condition()
	n = getRandomNumber(2);
	if (n == 0) then
		return CONDITION_POISONED
	else
		return CONDITION_NORMAL
	end
end
