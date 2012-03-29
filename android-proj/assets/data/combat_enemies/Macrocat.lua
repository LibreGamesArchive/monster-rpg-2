hp = 2000
attack = 400
defense = 300
speed = 120 
mdefense = 300 
luck = 150

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
		return COMBAT_CASTING, "Claw", 1, getRandomPlayer()
	end
end

function die()
end

