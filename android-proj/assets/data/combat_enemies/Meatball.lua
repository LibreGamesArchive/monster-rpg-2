hp = 160
attack = 30
defense = 40
speed = 20
mdefense = 0
luck = 10

strength = ELEMENT_NONE
weakness = ELEMENT_NONE

function start()
end

function get_attack_condition()
	n = getRandomNumber(5)
	if (n == 0) then
		battleSetInfoUnionI(p, 5) -- set paralyzeCount to 5
		return CONDITION_PARALYZED
	else
		return CONDITION_NORMAL
	end
end

function get_action(step)
	p = getRandomPlayer()
	return COMBAT_ATTACKING, 1, p
end

function die()
end


