hp = 1000 
attack = 245
defense = 200
speed = 30
mdefense = 240
luck = 40
float = 0

strength = ELEMENT_NONE
weakness = ELEMENT_NONE

function initId(id)
	myId = id
end

function start()
end

function get_action(step)
	if (getRandomNumber(2) == 0) then
		return COMBAT_CASTING, "Vampire", 1, getRandomPlayer()
	else
		return COMBAT_ATTACKING, 1, getRandomPlayer()
	end
end

function die()
end

