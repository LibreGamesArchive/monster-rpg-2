hp = 4000
attack = 400
defense = 200
speed = 70 
mdefense = 9999
luck = 100

strength = ELEMENT_FIRE
weakness = ELEMENT_ICE

transformed = false

function initId(i)
	id = i
end

function start()
end

function get_action()
	local n = getRandomNumber(2)
	if (not transformed) then
		if (n == 0) then
			return COMBAT_CASTING, "Fireball", 1, getRandomPlayer()
		else
			return COMBAT_CASTING, "Kiss of Death", 1, getRandomPlayer()
		end
	else
		if (n == 0) then
			return COMBAT_CASTING, "Breath of Fire", -1, nil
		else
			return COMBAT_CASTING, "Talon", 1, getRandomPlayer()
		end
	end
end

function die()
	transformed = true
end

function spell_has_no_effect(name)
	if (name == "Stun" or name == "Slow" or name == "Charm") then
		return true
	else
		return false
	end
end

