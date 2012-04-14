hp = 1000
attack = 300
defense = 230
speed = 35
mdefense = 100
luck = 40

strength = ELEMENT_NONE
weakness = ELEMENT_NONE

function initId(id)
	myId = id
end

function start()
end

function get_action(step)
	local n = getRandomNumber(2)
	if (n == 0) then
		return COMBAT_ATTACKING, 1, getRandomPlayer()
	else
		return COMBAT_CASTING, "Darkness2", -1, nil
	end
end

function die()
end

