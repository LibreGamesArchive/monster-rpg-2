hp = 1500
attack = 750
defense = 400
speed = 100
mdefense = 550
luck = 75

strength = ELEMENT_NONE
weakness = ELEMENT_NONE

function initId(id)
	myId = id
end

function start()
end

function get_action(step)
	if (getRandomNumber(4) == 0) then
		return COMBAT_CASTING, "Web", 1, getRandomPlayer()
	else
		return COMBAT_ATTACKING, 1, getRandomPlayer()
	end
end

function die()
end
