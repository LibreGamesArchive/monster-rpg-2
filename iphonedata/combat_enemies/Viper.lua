hp = 1000
attack = 260
defense = 230
speed = 60
mdefense = 200
luck = 60

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

