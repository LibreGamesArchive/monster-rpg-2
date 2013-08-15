hp = 1250
attack = 275
defense = 210
speed = 55
mdefense = 400
luck = 40

strength = ELEMENT_ELECTRICITY
weakness = ELEMENT_NONE

function initId(id)
	myId = id
end

function start()
end

function get_action(step)
	local n = getRandomNumber(3)
	if (n == 0) then
		return COMBAT_CASTING, "Bolt3", 1, getRandomPlayer()
	else
		return COMBAT_ATTACKING, 1, getRandomPlayer()
	end
end

function die()
end

