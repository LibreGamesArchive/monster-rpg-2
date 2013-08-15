num_enemies = 3
boss_fight = false 
experience=115
gold=100

function start()
	addEnemy("Gnome", 30, 60)
	addEnemy("Gnome", 50, 80)
	addEnemy("Gnome", 70, 100)
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

