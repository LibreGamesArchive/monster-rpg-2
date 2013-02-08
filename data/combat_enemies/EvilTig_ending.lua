hp = 999999999
attack = 9999
defense = 9999

if (enyHasWeapon()) then
	speed = 1
else
	speed = 999999
end

mdefense = 9999
luck = 35

strength = ELEMENT_NONE
weakness = ELEMENT_NONE

function start()
end

function get_action()
	local n = getRandomNumber(3)
	if (n == 0) then
		return COMBAT_CASTING, "Bolt3", 1, getRandomPlayer()
	elseif (n == 1) then
		return COMBAT_CASTING, "Fire3", 1, getRandomPlayer()
	else
		return COMBAT_CASTING, "Ice3", 1, getRandomPlayer()
	end
end

function die()
end

