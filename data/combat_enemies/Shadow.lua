hp = 2000
attack = 500
defense = 300
speed = 80
mdefense = 500 
luck = 100

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
	n = getRandomNumber(4);
	if (n == 0) then
		return CONDITION_SHADOW
	else
		return CONDITION_NORMAL
	end
end
