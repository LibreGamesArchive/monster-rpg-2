num_enemies = 1
boss_fight = true
experience=100
gold=100
water=true

function start()
	addEnemy("Monster", 50, 105)
end

function get_speech()
	return nil
end

function get_item()
	return ITEM_HEAL
end

