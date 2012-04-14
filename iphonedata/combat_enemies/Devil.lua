hp = 2000
attack = 450
defense = 450
speed = 100
mdefense = 500
luck = 50 

strength = ELEMENT_FIRE
weakness = ELEMENT_ICE

function initId(id)
	myId = id
end

function start()
end

function get_action(step)
	if (getRandomNumber(5) == 0) then
		return COMBAT_CASTING, "Touch of Death", 1, getRandomPlayer()
	else
		return COMBAT_ATTACKING, 1, getRandomPlayer()
	end
end

function die()
end
