hp = 3000
attack = 425
defense = 375
speed = 100 
mdefense = 800
luck = 100

strength = ELEMENT_NONE
weakness = ELEMENT_NONE

function initId(id)
	myId = id
end

function start()
end

function get_action(step)
	local n = getRandomNumber(9)
	if (n == 0) then
		return COMBAT_CASTING, "Fire3", 1, getRandomPlayer()
	elseif (n == 1) then
		return COMBAT_CASTING, "Ice3", 1, getRandomPlayer()
	elseif (n == 2) then
		return COMBAT_CASTING, "Bolt3", 1, getRandomPlayer()
	elseif (n == 3) then
		return COMBAT_CASTING, "Fire2", 1, getRandomPlayer()
	elseif (n == 4) then
		return COMBAT_CASTING, "Ice2", 1, getRandomPlayer()
	elseif (n == 5) then
		return COMBAT_CASTING, "Bolt2", 1, getRandomPlayer()
	elseif (n == 6) then
		return COMBAT_CASTING, "Charm", 1, getRandomPlayer()
	elseif (n == 7) then
		return COMBAT_CASTING, "Stone", 1, getRandomPlayer()
	elseif (n == 8) then
		return COMBAT_CASTING, "Slow", 1, getRandomPlayer()
	end
end

function die()
end
