num_enemies = 1
boss_fight = true
experience=1
gold=1

speechCount = 1

speech = {}

function get_speech()
	speechCount = speechCount + 1
	return speech[speechCount-1]
end

function start()
	addEnemy("EvilTig_ending", 32, 80)
	if (not enyHasWeapon()) then
		speech = {
			"Tiggy: You have no weapon?!\n",
			"Hahahaha!\n"
		}
	end
end

function get_item()
	return -1
end


