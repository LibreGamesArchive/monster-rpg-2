hp = 300 
attack = 150
defense = 180
speed = 40
mdefense = 130
luck = 35

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
	n = getRandomNumber(4);
	if (n == 0) then
		return CONDITION_POISONED
	elseif (n == 1) then
		return CONDITION_PARALYZED
	else
		return CONDITION_NORMAL
	end
end
