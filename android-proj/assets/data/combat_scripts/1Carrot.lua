num_enemies = 1
boss_fight = false
experience=300
gold=200
can_run = false

function start()
	addEnemy("Carrot", 60, 72)
end

function get_speech()
	return nil
end

function get_item()
	return ITEM_FLINT
end

