num_enemies = 2
boss_fight = false 
experience=300
gold=180

function start()
	addEnemy("Magmod", 40, 85)
	addEnemy("Magmod", 80, 100)
end

function get_speech()
	return nil
end

function get_item()
	return -1
end

