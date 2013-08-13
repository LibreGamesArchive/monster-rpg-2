hp = 1000
attack = 350
defense = 300
speed = 67
mdefense = 300
luck = 50
float = 0

strength = ELEMENT_NONE
weakness = ELEMENT_NONE

stage = 0

STAGE_INIT     = 0
STAGE_FORWARD  = 1
STAGE_BACKWARD = 2

SPEED = 3 / 10
sucked = false

function initId(i)
	id = i
end

function start()
	preloadSFX("buzz.ogg")
	preloadSFX("suck.ogg")
end

function get_attack_condition()
	n = getRandomNumber(2);
	if (n == 0) then
		return CONDITION_POISONED
	else
		return CONDITION_NORMAL
	end
end

function get_action(step)
	if (stage == STAGE_INIT) then
		if (getRandomNumber(2) == 0) then
			return COMBAT_ATTACKING, 1, getRandomPlayer()
		else
			loadPlayDestroy("buzz.ogg")
			stage = stage + 1
			delay = 0
			start_x = battleGetX(id)
			start_y = battleGetY(id)
			target = getRandomPlayer()
			loc = battleGetLocation(id)
			battleSetSubAnimation(id, "forward")
			if (loc == LOCATION_LEFT) then
				dest_x = battleGetX(target)-battleGetWidth(id)/2
			else
				dest_x = battleGetX(target)+battleGetWidth(id)/2
			end
			dest_y = battleGetY(target)+battleGetHeight(target)/2
		end
	elseif (stage == STAGE_FORWARD) then
		cx = battleGetX(id)
		cy = battleGetY(id)

		if (cx < dest_x) then
			cx = cx + (SPEED * step)
			if (cx > dest_x) then
				cx = dest_x
			end
		elseif (cx > dest_x) then
			cx = cx - (SPEED * step)
			if (cx < dest_x) then
				cx = dest_x
			end
		end
		if (cy < dest_y) then
			cy = cy + (SPEED * step)
			if (cy > dest_y) then
				cy = dest_y
			end
		elseif (cy > dest_y) then
			cy = cy - (SPEED * step)
			if (cy < dest_y) then
				cy = dest_y
			end
		end

		battleSetX(id, cx)
		battleSetY(id, cy)

		battleResortEntity(id)

		if (cx == dest_x and cy == dest_y) then
			if (not sucked) then
				sucked = true
				loadPlayDestroy("suck.ogg")
				if (loc == LOCATION_LEFT) then
					battleAddMessage(MESSAGE_LEFT, "Brain Drain", 3000)
				else
					battleAddMessage(MESSAGE_RIGHT, "Brain Drain", 3000)
				end
			end
			delay = delay + step
			if (delay > 2000) then
				battleKillCombatant(target)
				stage = stage + 1
				battleSetSubAnimation(id, "backward")
				loadPlayDestroy("buzz.ogg")
				sucked =  false
			end
		end
	elseif (stage == STAGE_BACKWARD) then
		cx = battleGetX(id)
		cy = battleGetY(id)

		if (cx < start_x) then
			cx = cx + (SPEED * step)
			if (cx > start_x) then
				cx = start_x
			end
		elseif (cx > start_x) then
			cx = cx - (SPEED * step)
			if (cx < start_x) then
				cx = start_x
			end
		end
		if (cy < start_y) then
			cy = cy + (SPEED * step)
			if (cy > start_y) then
				cy = start_y
			end
		elseif (cy > start_y) then
			cy = cy - (SPEED * step)
			if (cy < start_y) then
				cy = start_y
			end
		end

		battleSetX(id, cx)
		battleSetY(id, cy)

		battleResortEntity(id)

		if (cx == start_x and cy == start_y) then
			battleSetSubAnimation(id, "stand")
			stage = STAGE_INIT
			return COMBAT_SKIP
		end
	end

	return COMBAT_BUSY
end

function die()
end

