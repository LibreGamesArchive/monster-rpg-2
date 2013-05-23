num_enemies = 3
boss_fight = false 
experience=150
gold=150

function start()
	addEnemy("Mage", 30, 70)
	addEnemy("Mage", 30, 100)
	addEnemy("Halberd", 75, 85)
end

function get_speech()
	return nil
end

function get_item()
	return -1
end

