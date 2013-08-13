hp = 3000
attack = 300
defense = 200
speed = 45
mdefense = 9999
luck = 40

strength = ELEMENT_NONE
weakness = ELEMENT_NONE

function initId(id)
	myId = id
end

function start()
end

function added()
	setCombatantAnimationSetPrefix(myId, "staff_")
	battleSetSubAnimation(myId, "stand")
end

function get_action(step)
	if (battleGetNumEnemies() == 1) then
		return COMBAT_CASTING, "Fire3", 1, getRandomPlayer()
	else
		return COMBAT_ATTACKING, 1, getRandomPlayer()
	end
end

function die()
end

function getDamage(who, damage)
	if (who == "Minion") then
		return damage
	else
		return 1
	end
end

