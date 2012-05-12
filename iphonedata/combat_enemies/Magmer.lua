hp = 2600
attack = 350
defense = 280 
speed = 80
mdefense = 280
luck = 75
float = 0

strength = ELEMENT_FIRE
weakness = ELEMENT_ICE

function initId(id)
	myId = id
end

function start()
	preloadSpellSFX("Boulder")
end

function get_action(step)
	local n = getRandomNumber(2);
	if (n == 0) then
		return COMBAT_CASTING, "Boulder", 1, getRandomPlayer()
	else
		return COMBAT_ATTACKING, 1, getRandomPlayer()
	end
end

function die()
end
