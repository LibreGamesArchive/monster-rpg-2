hp = 700 
attack = 210
defense = 180
speed = 45
mdefense = 300
luck = 50 

strength = ELEMENT_NONE
weakness = ELEMENT_NONE

function initId(id)
	myId = id
end

function start()
end

function get_action(step)
	local n = getRandomNumber(3)
	if (n == 0) then
		return COMBAT_CASTING, "Twister", 1, getRandomPlayer()
	elseif (n == 1) then
		return COMBAT_CASTING, "Slow", 1, getRandomPlayer()
	else
		return COMBAT_ATTACKING, 1, getRandomPlayer()
	end
end

function die()
end

