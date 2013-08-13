hp = 5000
attack = 280
defense = 250 
speed = 55
mdefense = 300
luck = 60 
explodes = true

strength = ELEMENT_NONE
weakness = ELEMENT_FIRE

local player_ids = {}

local attacks_in_a_row = 0

function initId(id)
	myId = id
end

function numCharmed()
	local count = 0

	for i=1,#player_ids do
		if (battleGetEntityCondition(player_ids[i]) == CONDITION_CHARMED) then
			count = count + 1
		end
	end

	return count
end

function start()
	local id = battleGetNextEntity(myId)

	while (not (id == myId)) do
		local t = battleGetEntityType(id)
		if (t == COMBATENTITY_TYPE_PLAYER) then
			player_ids[#player_ids+1] = id
		end
		id = battleGetNextEntity(id)
	end
end

function get_action(step)
	local n = getRandomNumber(2)
	if (n == 0 or attacks_in_a_row > 2) then
		attacks_in_a_row = 0
		local nc = numCharmed()
		if (#player_ids > 1 and nc < #player_ids/2) then
			local start = getRandomNumber(#player_ids)+1
			local n = start
			local found
			repeat
				if (battleGetEntityHP(player_ids[n]) > 0 and not (battleGetEntityCondition(player_ids[n]) == CONDITION_CHARMED)) then
					found = n
					n = start - 999 -- hack
					break
				end
				n = n + 1
				if (n > #player_ids) then
					n = 1
				end
			until n == start
			if (n == start) then
				return COMBAT_ATTACKING, 1, getRandomPlayer()
			else
				return COMBAT_CASTING, "Charm", 1, player_ids[found]
			end
		else
			return COMBAT_ATTACKING, 1, getRandomPlayer()
		end
	else
		attacks_in_a_row = attacks_in_a_row + 1
		return COMBAT_ATTACKING, 1, getRandomPlayer()
	end
end

function die()
	addEnemy("Spirit", battleGetX(myId), battleGetY(myId)-10, true)
end

function spell_has_no_effect(name)
	if (name == "Stun" or name == "Slow" or name == "Charm") then
		return true
	else
		return false
	end
end

