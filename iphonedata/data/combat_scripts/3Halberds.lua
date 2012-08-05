num_enemies = 3
boss_fight = false 
experience=75
gold=100
can_run = false

function start()
	addEnemy("Halberd", 30, 60)
	addEnemy("Halberd", 50, 80)
	addEnemy("Halberd", 70, 100)
end

function get_speech()
	return nil
end

function get_item()
	if (getRandomNumber(20) == 0) then
		return ITEM_CURE2
	elseif (getRandomNumber(20) == 0) then
		return ITEM_HEAL
	else
		return -1
	end
end

