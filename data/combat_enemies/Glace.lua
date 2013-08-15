hp = 2500
attack = 350
defense = 300
speed = 100
mdefense = 350
luck = 75
float = 0

strength = ELEMENT_ICE
weakness = ELEMENT_FIRE

function initId(id)
	myId = id
end

function start()
end

function get_action(step)
	local n = getRandomNumber(3)
	if (n == 0) then
		return COMBAT_CASTING, "Ice3", 1, getRandomPlayer()
	elseif (n == 1) then
		return COMBAT_CASTING, "Ice2", 1, getRandomPlayer()
	else
		return COMBAT_CASTING, "Ice1", 1, getRandomPlayer()
	end
end

function die()
end
