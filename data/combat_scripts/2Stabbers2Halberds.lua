num_enemies = 4
boss_fight = false 
experience=130
gold=140

function start()
	addEnemy("Stabber", 30, 70)
	addEnemy("Stabber", 30, 100)
	addEnemy("Halberd", 75, 100)
	addEnemy("Halberd", 75, 70)
end

function get_speech()
	return nil
end

function get_item()
	return -1
end

