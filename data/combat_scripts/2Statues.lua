num_enemies = 2
boss_fight = false 
experience=1
gold=1
can_run = false

function start()
	addEnemy("Statue", 42, 80)
	addEnemy("Statue", 62, 100)
end

function get_speech()
	return nil
end

function get_item()
	return -1
end

