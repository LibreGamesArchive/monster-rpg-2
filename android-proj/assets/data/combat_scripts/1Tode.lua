num_enemies = 1
boss_fight = true
experience=1000
gold=1000

function start()
	addEnemy("Tode", 45, 105)
end

speech = {
	"Tode: I may be a vegetarian...\n",
	"Tode: But I will enjoy eating you!\n",
}

speechCount = 1

function get_speech()
	speechCount = speechCount + 1
	return speech[speechCount-1]
end

function get_item()
	return -1
end

