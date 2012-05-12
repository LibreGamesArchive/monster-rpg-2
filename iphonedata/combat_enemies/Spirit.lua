hp = 3000
attack = 300
defense = 250 
speed = 55
mdefense = 300
luck = 60 

strength = ELEMENT_ELECTRICITY
weakness = ELEMENT_NONE

function initId(id)
	myId = id
end

function start()
	preloadSpellSFX("Arc")
end

function get_action(step)
	local n = getRandomNumber(2)
	if (n == 0) then
		return COMBAT_CASTING, "Arc", 1, getRandomPlayer()
	else
		return COMBAT_ATTACKING, 1, getRandomPlayer()
	end
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


