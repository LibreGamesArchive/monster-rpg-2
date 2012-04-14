hp = 6000
attack = 425
defense = 300
speed = 80
mdefense = 300
luck = 80

strength = ELEMENT_FIRE
weakness = ELEMENT_ICE

function initId(id)
	myId = id
end

function start()
end

function get_action(step)
	local n = getRandomNumber(2)
	if (n == 0) then
		return COMBAT_ATTACKING, 1, getRandomPlayer()
	elseif (n == 1) then
		return COMBAT_CASTING, "Blaze", -1, nil
	end
end

function die()
	local players = {}
	local n = 1
	local i
	i = battleGetNextEntity(myId)
	while (not (i == myId)) do
		if (battleGetEntityType(i) == COMBATENTITY_TYPE_PLAYER) then
			if (battleGetEntityHP(i) > 0) then
				players[n] = i
				n = n + 1
			end
		end
		i = battleGetNextEntity(i)
	end
	for i=1,#players do
		battleDoAttack(myId, players[i])
		battleAddFire3Effect(players[i])
	end
	if (n > 1) then
		loadPlayDestroy("Fire3.ogg")
	end
end
