hp = 2000
attack = 500
defense = 400
speed = 100
mdefense = 450
luck = 70

strength = ELEMENT_NONE
weakness = ELEMENT_NONE

function initId(id)
	myId = id
end

function start()
end

function get_action(step)
	if (getRandomNumber(5) == 0) then
		return COMBAT_CASTING, "Darkness3", -1, nil
	else
		return COMBAT_ATTACKING, 1, getRandomPlayer()
	end
end

function die()
end
