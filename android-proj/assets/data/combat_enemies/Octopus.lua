hp = 1000 
attack = 160
defense = 120
speed = 40
mdefense = 100
luck = 40

strength = ELEMENT_NONE
weakness = ELEMENT_ELECTRICITY

skip = true
target = 0

function initId(id)
	myId = id
end

function start()
end

function get_action(step)
	skip = true
	return COMBAT_ATTACKING, 1, target
end

function die()
end

function retaliate(n)
	skip = false
	target = n
end
