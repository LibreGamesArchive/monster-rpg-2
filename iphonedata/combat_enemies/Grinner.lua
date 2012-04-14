hp = 250
attack = 2000
defense = 300
speed = 95
mdefense = 9999
luck = 150

strength = ELEMENT_FIRE
weakness = ELEMENT_ICE

function initId(id)
	myId = id
end

function start()
end

function get_action(step)
	return COMBAT_CASTING, "Fire3", 1, getRandomPlayer()
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
	local p = getRandomNumber(n-1) + 1
	battleDoAttack(myId, players[p])
	battleAddExplodeEffect(players[p])
end
