num_enemies = 3
boss_fight = false 
experience=40
gold=30

function start()
	addEnemy("Shroom", 30, 60)
	addEnemy("Bud", 60, 80)
	addEnemy("Shroom", 30, 100)
end

function get_speech()
	return nil
end

function get_item()
	if (getRandomNumber(20) == 0) then
		return ITEM_HEAL
	else
		return -1
	end
end

