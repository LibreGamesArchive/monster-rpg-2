hp = 10
attack = 10
defense = 10
speed = 10
mdefense = 0
luck = 10 

strength = ELEMENT_NONE
weakness = ELEMENT_NONE

function start()
end

function get_action(step)
	return COMBAT_ATTACKING, 1, getRandomPlayer()
end


function die()
end

