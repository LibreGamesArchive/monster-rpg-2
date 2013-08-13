hp = 300
attack = 375
defense = 375 
speed = 110
mdefense = 500
luck = 50

strength = ELEMENT_NONE
weakness = ELEMENT_NONE

function initId(id)
	myId = id
end

function start()
	preloadSpellSFX("UFO")
end

function get_action(step)
	if (getRandomNumber(2) == 0) then
		return COMBAT_CASTING, "UFO", 1, getRandomPlayer()
	else
		return COMBAT_ATTACKING, 1, getRandomPlayer()
	end
end

function die()
end

