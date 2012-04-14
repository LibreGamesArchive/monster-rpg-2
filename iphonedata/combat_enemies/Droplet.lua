hp = 400 
attack = 120
defense = 180
speed = 35
mdefense = 100
luck = 35
float = 0

strength = ELEMENT_NONE
weakness = ELEMENT_ELECTRICY

function initId(id)
	myId = id
end

function start()
end

function get_action(step)
	if (getRandomNumber(2) == 0) then
		return COMBAT_CASTING, "Torrent", -1, nil
	else
		return COMBAT_CASTING, "Punch", 1, getRandomPlayer()
	end
end

function die()
end

