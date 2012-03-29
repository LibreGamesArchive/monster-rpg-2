hp = 150
attack = 35
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
	playPreloadedSample("slime.ogg")

	addEnemy("MiniSlime", battleGetX(myId)+5, battleGetY(myId)-15, true)
	addEnemy("MiniSlime", battleGetX(myId)+20, battleGetY(myId)-5, true)
end

