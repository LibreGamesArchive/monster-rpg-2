hp = 999999999
attack = 9999
defense = 9999
speed = 1
mdefense = 9999
luck = 30 

strength = ELEMENT_NONE
weakness = ELEMENT_NONE

function initId(id)
	myId = id
end

function start()
	preloadSpellSFX("Beam")
end

function get_action(step)
	return COMBAT_CASTING, "Beam", 1, getRandomPlayer()
end

function die()
end

function spell_has_no_effect(name)
	if (name == "Stun" or name == "Slow" or name == "Charm") then
		return true
	else
		return false
	end
end

