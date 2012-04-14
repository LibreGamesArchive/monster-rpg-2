hp = 2500
attack = 200
defense = 300
speed = 80
mdefense = 300
luck = 80
float = 0

strength = ELEMENT_FIRE
weakness = ELEMENT_ICE

function initId(id)
	myId = id
end

function start()
end

function get_action(step)
	local n = getRandomNumber(2);
	if (n == 0) then
		return COMBAT_CASTING, "Fire2", -1, nil
	else
		return COMBAT_CASTING, "Fire3", 1, getRandomPlayer()
	end
end

function die()
end
