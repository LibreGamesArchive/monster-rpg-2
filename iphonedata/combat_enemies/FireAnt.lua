hp = 2000
attack = 350
defense = 250
speed = 70
mdefense = 300
luck = 60

strength = ELEMENT_FIRE
weakness = ELEMENT_ICE

function initId(id)
	myId = id
end

function start()
end

function get_action(step)
	local n = getRandomNumber(2)
	if (n == 0) then
		return COMBAT_CASTING, "Fireball", -1, nil
	else
		return COMBAT_ATTACKING, 1, getRandomPlayer()
	end
end

function die()
end
