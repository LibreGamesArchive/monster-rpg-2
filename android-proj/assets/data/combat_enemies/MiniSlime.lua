hp = 50
attack = 40
defense = 40
speed = 10
mdefense = 50
luck = 10 

strength = ELEMENT_NONE
weakness = ELEMENT_NONE

function initId(id)
	myId = id
end

function start()
end

function get_action(step)
	return COMBAT_ATTACKING, 1, getRandomPlayer()
end

function die()
	player = getRandomPlayer()
	battleDoAttack(myId, player)
	battleAddSlimeEffect(player)
	playPreloadedSample("slime.ogg")
end

