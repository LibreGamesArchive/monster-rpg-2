hp = 10000
attack = 650
defense = 500
speed = 100
mdefense = 600
luck = 100

strength = ELEMENT_NONE
weakness = ELEMENT_NONE

function initId(id)
	myId = id
end

function start()
	preloadSpellSFX("Swallow")
	preloadSpellSFX("Puke")
	preloadSpellSFX("BellyAcid")
end

function get_action(step)
	local swallowed = 0
	local incapacitated = 0
	local id = battleGetNextEntity(myId)
	while (not (id == myId)) do
		if (battleGetEntityCondition(id) == CONDITION_SWALLOWED) then
			swallowed = swallowed + 1
			incapacitated = incapacitated + 1
		elseif (battleGetEntityHP(id) <= 0) then
			incapacitated = incapacitated + 1
		end
		id = battleGetNextEntity(id)
	end

	if (battleGetNumEnemies() == 1) then
		local id = battleGetNextEntity(myId)
		local can_swallow = {}
		while (not (id == myId)) do
			if (battleGetEntityType(id) == COMBATENTITY_TYPE_PLAYER) then
				if (not (battleGetEntityCondition(id) == CONDITION_SWALLOWED)) then
					if (battleGetEntityHP(id) > 0) then
						can_swallow[#can_swallow+1] = id
					end
				end
			end
			id = battleGetNextEntity(id)
		end
		if ((#can_swallow > 1) and (getRandomNumber(3) == 0)) then
			local i = getRandomNumber(#can_swallow)+1
			return COMBAT_CASTING, "Swallow", 1, can_swallow[i]
		else
			if (incapacitated >= 4 or (swallowed > 0 and getRandomNumber(3) == 0)) then
				return COMBAT_CASTING, "Puke", 0, nil
			else
				local n = getRandomPlayer()
				if (n == -1) then
					return COMBAT_SKIP
				else
					if (getRandomNumber(2) == 1) then
						return COMBAT_ATTACKING, 1, n
					else
						return COMBAT_CASTING, "BellyAcid", 1, n
					end
				end
			end
		end
	else
		local n = getRandomPlayer()
		if (n == -1) then
			return COMBAT_SKIP
		else
			if (getRandomNumber(2) == 1) then
				return COMBAT_ATTACKING, 1, n
			else
				return COMBAT_CASTING, "BellyAcid", 1, n
			end
		end
	end
end

function die()
	local id = battleGetNextEntity(myId)
	while (not (id == myId)) do
		if (battleGetEntityType(id) == COMBATENTITY_TYPE_PLAYER) then
			if (battleGetEntityCondition(id) == CONDITION_SWALLOWED) then
				battleSetEntityCondition(id, CONDITION_NORMAL)
			end
		end
		id = battleGetNextEntity(id)
	end
end

function get_attack_condition()
	return CONDITION_POISONED
end

function spell_has_no_effect(name)
	if (name == "Stun" or name == "Slow" or name == "Charm") then
		return true
	else
		return false
	end
end
