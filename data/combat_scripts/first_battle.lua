num_enemies = 1
boss_fight = true
experience=0
gold=0

function start()
	addEnemy("EvilTig_beginning", 32, 80)
end

speech = {
	"Eny: Tiggy, put down the staff!\n",
	"Tig: You'll never take it!\n",
	nil,
	nil,
	"Eny: Tiggy, listen to me!!!\n",
	"Tig: ...\n",
	nil,
	nil,
	"Eny: Tiggy... please...\n"
}

speechCount = 1

function get_speech()
	speechCount = speechCount + 1
	return speech[speechCount-1]
end

function get_item()
	return -1
end

