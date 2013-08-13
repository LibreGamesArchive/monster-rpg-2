hp = 800
attack = 150
defense = 80
speed = 20
mdefense = 20
luck = 25

strength = ELEMENT_NONE
weakness = ELEMENT_NONE

first = true

function start()
	preloadSpellSFX("Stomp")
	preloadSpellSFX("Spray")
end

function get_action()
	if (first) then
		first = false
		return COMBAT_CASTING, "Stomp", -1, nil
	else
		n = getRandomNumber(3)
		if (n == 0) then
			return COMBAT_CASTING, "Stomp", -1, nil
		else
			return COMBAT_CASTING, "Spray", 1, getRandomPlayer()
		end
	end
end

function die()
end

