hp = 2300
attack = 420
defense = 400
speed = 80
mdefense = 300 
luck = 70

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
		return COMBAT_CASTING, "Laser", 1, getRandomPlayer()
	end
end

function die()
end

