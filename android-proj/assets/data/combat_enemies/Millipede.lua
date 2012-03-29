hp = 1600
attack = 275
defense = 230
speed = 80
mdefense = 400
luck = 60

strength = ELEMENT_NONE
weakness = ELEMENT_NONE

function initId(id)
	myId = id
end

function start()
end

function get_action(step)
	local targets = {}
	
	for i=1,4 do
		local p = getRandomPlayer()
		local alreadyExists = false
		for j=1,#targets do
			if (targets[j] == p) then
				alreadyExists = true
				break
			end
		end
		if (not alreadyExists) then
			targets[#targets+1] = p
		end
	end

	return COMBAT_ATTACKING, #targets, unpack(targets, 1, #targets)
end

function die()
end

