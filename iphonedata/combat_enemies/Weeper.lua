hp = 55
attack = 28
defense = 15
speed = 30
mdefense = 0
luck = 15

strength = ELEMENT_NONE
weakness = ELEMENT_NONE

function start()
	preloadSpellSFX("Weep")
end

function get_action()
	n = getRandomNumber(10)
	if (n <= 2) then
		return COMBAT_ATTACKING, 1, getRandomPlayer()
	else
		return COMBAT_CASTING, "Weep", 1, getRandomPlayer()
	end
end

function die()
end

