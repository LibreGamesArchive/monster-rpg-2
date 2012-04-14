hp = 8000
attack = 300
defense = 250
speed = 80
mdefense = 500
luck = 60
explodes = true

strength = ELEMENT_NONE
weakness = ELEMENT_FIRE

function initId(id)
	myId = id
end

function start()
end

function get_action(step)
	local n = getRandomNumber(5)
	if (n == 0 or n == 1) then
		return COMBAT_CASTING, "Whip", 1, getRandomPlayer()
	elseif (n == 2 or n == 3) then
		return COMBAT_CASTING, "Acorns", -1, nil
	else
		return COMBAT_ATTACKING, 1, getRandomPlayer()
	end
end

function die()
	addEnemy("Stump", battleGetX(myId), battleGetY(myId), true)
end

function spell_has_no_effect(name)
	if (name == "Stun" or name == "Slow" or name == "Charm") then
		return true
	else
		return false
	end
end

