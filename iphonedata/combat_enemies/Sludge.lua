hp = 250
attack = 75
defense = 75
speed = 20
mdefense = 100
luck = 10 

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
		return COMBAT_CASTING, "Sludge", 1, getRandomPlayer()
	end
end

function die()
	loadPlayDestroy("slime.ogg")

	addEnemy("MiniSludge", battleGetX(myId)+5, battleGetY(myId)-15, true)
	addEnemy("MiniSludge", battleGetX(myId)+20, battleGetY(myId)-5, true)
end

