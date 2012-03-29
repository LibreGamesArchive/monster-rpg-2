hp = 2000
attack = 300
defense = 200
speed = 45
mdefense = 200
luck = 40

strength = ELEMENT_NONE
weakness = ELEMENT_NONE

function initId(id)
	myId = id
end

function start()
end

function get_action(step, condition)
	if (condition == CONDITION_CHARMED) then
		local n = battleFindCombatant("Rider")
		if (n == -1) then
			return COMBAT_ATTACKING, 1, getRandomEnemy()
		else
			return COMBAT_ATTACKING, 1, n
		end
	else
		return COMBAT_ATTACKING, 1, getRandomPlayer()
	end
end

function die()
end

