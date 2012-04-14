num_enemies = 3
boss_fight = true
experience=500
gold=500

n = 0

function start()
	addEnemy("Minion", 75, 80)
	addEnemy("Rider", 25, 80)
	addEnemy("Minion", 80, 105)
end

function get_speech()
	if (n == 0) then
		n = 1
		return "Rider: Hoohoohoohahaha!\n"
	else
		return nil
	end
end

function get_item()
	return ITEM_CURE3
end

