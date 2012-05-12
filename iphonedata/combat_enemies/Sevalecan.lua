hp = 3000
attack = 520
defense = 300
speed = 100
mdefense = 350
luck = 75

strength = ELEMENT_NONE
weakness = ELEMENT_NONE

function initId(id)
	myId = id
end

function start()
	preloadSpellSFX("MachineGun")
end

function get_action(step)
	return COMBAT_CASTING, "MachineGun", 1, getRandomPlayer()
end

function die()
end
