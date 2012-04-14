hp = 250
attack = 90
defense = 100
speed = 22
mdefense = 10
luck = 10 

strength = ELEMENT_NONE
weakness = ELEMENT_NONE

STAGE_INIT = 0
STAGE_FORWARD = 1
STAGE_BACKWARD = 2

stage = STAGE_INIT

SPIN_SPEED = math.frac2fix(5, 100)
FORWARD_TIME = 250
BACKWARD_TIME = 300
PEAK_HEIGHT = 50


function initId(id)
	myId = id
end

function start()
end

function get_action(step)
	if (stage == STAGE_INIT) then
		ox = 0
		oy = 0
		stage = stage + 1
		battleSetSubAnimation(myId, "spin")
		loadPlayDestroy("spin.ogg")
		target = getRandomPlayer()
		if (battleGetLocation(target) == LOCATION_LEFT) then
			dx = (battleGetX(target) + (battleGetWidth(target)/2) + 20) - battleGetX(myId)
		else
			dx = (battleGetX(target) - (battleGetWidth(target)/2) - 20) - battleGetX(myId)
		end
		dy = (battleGetY(target) - battleGetHeight(target)/2 + 23) - battleGetY(myId)
		angle = 0
		count = 0
	elseif (stage == STAGE_FORWARD) then
		angle = math.fixadd(angle, math.fixmul(math.itofix(step), SPIN_SPEED))
		count = count + step
		ox = math.fixtoi(math.fixmul(math.fixdiv(math.itofix(count), math.itofix(FORWARD_TIME)), math.itofix(dx)))
		oy = math.fixtoi(math.fixmul(math.fixdiv(math.itofix(count), math.itofix(FORWARD_TIME)), math.itofix(dy)))
		battleSetOx(myId, ox)
		battleSetOy(myId, oy)
		battleSetAngle(myId, angle)
		if (count >= FORWARD_TIME) then
			ox = dx
			oy = dy
			count = 0
			stage = stage + 1
		end
	elseif (stage == STAGE_BACKWARD) then
		angle = math.fixadd(angle, math.fixmul(math.itofix(step), SPIN_SPEED))
		count = count + step
		ox = math.fixtoi(math.fixmul(math.fixsub(math.itofix(1), math.fixdiv(math.itofix(count), math.itofix(BACKWARD_TIME))), math.itofix(dx)))
		oy = math.fixtoi(math.fixmul(math.fixsub(math.itofix(1), math.fixdiv(math.itofix(count), math.itofix(BACKWARD_TIME))), math.itofix(dy)))
		local tmp = math.fixsin(math.fixmul(math.fixdiv(math.itofix(count), math.itofix(BACKWARD_TIME)), math.fixpi))
		oy = math.fixtoi(math.fixsub(math.itofix(oy), math.fixmul(tmp, math.itofix(PEAK_HEIGHT))))
		battleSetOx(myId, ox)
		battleSetOy(myId, oy)
		battleSetAngle(myId, angle)
		if (count >= BACKWARD_TIME) then
			battleSetOx(myId, 0)
			battleSetOy(myId, 0)
			battleSetAngle(myId, 0)
			stage = STAGE_INIT
			battleSetSubAnimation(myId, "stand")
			return COMBAT_ATTACKING, 1, target
		end
	end

	return COMBAT_BUSY
end

function die()
end

