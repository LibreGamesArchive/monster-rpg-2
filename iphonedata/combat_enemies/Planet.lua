hp = 2000
attack = 350
defense = 350
speed = 110 
mdefense = 350 
luck = 100

strength = ELEMENT_NONE
weakness = ELEMENT_NONE

function initId(id)
	myId = id
end

function start()
	preloadSpellSFX("Orbit")
end

function get_action(step)
	if (getRandomNumber(2) == 0) then
		return COMBAT_ATTACKING, 1, getRandomPlayer()
	else
		return COMBAT_CASTING, "Orbit", -1, nil
	end
end

function die()
end
