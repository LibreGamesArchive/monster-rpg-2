hp = 30
attack = 15
defense = 10
speed = 10
mdefense = 0
luck = 10 

strength = ELEMENT_NONE
weakness = ELEMENT_NONE

function initId(id)
	myId = id
end

function start()
	preloadSFX("slime.ogg")
end

function get_action(step)
	return COMBAT_ATTACKING, 1, getRandomPlayer()
end

function die()
	loadPlayDestroy("slime.ogg")

	addEnemy("MiniGoo", battleGetX(myId)+5, battleGetY(myId)-15, true)
	addEnemy("MiniGoo", battleGetX(myId)+20, battleGetY(myId)-5, true)
end

