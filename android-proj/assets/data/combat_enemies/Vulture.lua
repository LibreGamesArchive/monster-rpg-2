hp = 2000
attack = 333
defense = 260
speed = 90
mdefense = 400
luck = 50

strength = ELEMENT_NONE
weakness = ELEMENT_NONE

function initId(id)
	myId = id
end

function start()
	preloadSpellSFX("Drop")
end

function get_action(step)
	local n = getRandomNumber(3)
	if (n == 0) then
		return COMBAT_ATTACKING, 1, getRandomPlayer()
	else
		return COMBAT_CASTING, "Drop", 1, getRandomPlayer()
	end
end

function die()
end
