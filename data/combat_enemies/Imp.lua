hp = 200
attack = 90
defense = 120
speed = 20
mdefense = 0
luck = 20 

strength = ELEMENT_NONE
weakness = ELEMENT_NONE

function initId(id)
	myId = id
end

function start()
end

function get_action(step)
	target1 = getRandomPlayer()
	target2 = getRandomPlayer()
	if (not (target1 == target2)) then
		return COMBAT_ATTACKING, 2, target1, target2
	else
		return COMBAT_ATTACKING, 1, target1
	end
end

function die()
end

