hp = 700
attack = 280
defense = 230
speed = 55
mdefense = 300
luck = 60 

strength = ELEMENT_NONE
weakness = ELEMENT_NONE

local player_ids = {}

function initId(id)
	myId = id
end

function numMushrooms()
	local count = 0

	for i=1,#player_ids do
		if (battleGetEntityCondition(player_ids[i]) == CONDITION_MUSHROOM) then
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

function get_attack_condition()
	if (numMushrooms() < #player_ids/2) then
		if (getRandomNumber(3) == 0) then
			return CONDITION_MUSHROOM
		else
			return CONDITION_NORMAL
		end
	else
		return CONDITION_NORMAL
	end
end

function get_action(step)
	local n = getRandomNumber(2)
	if (n == 0) then
		return COMBAT_ATTACKING, 1, getRandomPlayer()
	else
		return COMBAT_CASTING, "Daisy", 1, getRandomPlayer()
	end
end

function die()
end

