hp = 250
attack = 120
defense = 120
speed = 30
mdefense = 100
luck = 5

strength = ELEMENT_NONE
weakness = ELEMENT_NONE

function initId(id)
	myId = id
end

function start()
end

function get_action(step)
	local n = randint(2)
	if (n == 0) then
		return COMBAT_ATTACKING, 1, getRandomPlayer()
	else
		return COMBAT_CASTING, "Rend", 1, getRandomPlayer()
	end
end

function die()
end

