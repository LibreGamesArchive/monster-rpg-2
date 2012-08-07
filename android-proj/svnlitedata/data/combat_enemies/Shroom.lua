hp = 50
attack = 30
defense = 10
speed = 10
mdefense = 0
luck = 5

strength = ELEMENT_NONE
weakness = ELEMENT_NONE

function get_attack_condition()
	n = getRandomNumber(2);
	if (n == 0) then
		return CONDITION_POISONED
	else
		return CONDITION_NORMAL
	end
end

function start()
end

function get_action()
	return COMBAT_ATTACKING, 1, getRandomPlayer()
end

function die()
end

