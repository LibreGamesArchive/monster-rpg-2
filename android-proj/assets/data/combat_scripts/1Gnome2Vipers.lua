num_enemies = 3
boss_fight = false 
experience=190
gold=150

function start()
	addEnemy("Gnome", 30, 80)
	addEnemy("Viper", 65, 75)
	addEnemy("Viper", 65, 100)
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

