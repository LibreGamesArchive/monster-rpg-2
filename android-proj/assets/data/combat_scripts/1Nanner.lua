num_enemies = 1
boss_fight = false
experience=250
gold=150
can_run = false

function start()
	addEnemy("Nanner", 60, 80)
end

function get_speech()
	return nil
end

function get_item()
	return -1
end

