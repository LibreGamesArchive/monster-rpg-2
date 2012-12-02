hp = 2000
attack = 350
defense = 1000
speed = 50
mdefense = 400
luck = 50

strength = ELEMENT_NONE
weakness = ELEMENT_NONE

stage = 0

STAGE_INIT     = 0
STAGE_JUMPING  = 1
STAGE_FALLING  = 2
STAGE_POUNDING = 3
STAGE_BACKUP   = 4
STAGE_BACKDOWN = 5

ox = 0
oy = 0

SPEED = 3 / 10
MAX_HEIGHT = -200

function initId(i)
	id = i
end

function start()
	preloadSFX("jump.ogg")
	preloadSFX("fall.ogg")
	preloadSFX("Thud.ogg")
end

function get_action(step)
	if (stage == STAGE_INIT) then
		loadPlayDestroy("jump.ogg")
		stage = stage + 1
		delay = 0
		start_x = battleGetX(id)
		start_y = battleGetY(id)
	elseif (stage == STAGE_JUMPING) then
		oy = fixsub(oy, fixmul(SPEED, step))
		battleSetOy(id, fixtoi(oy))
		if (fixtoi(oy) < MAX_HEIGHT) then
			target = getRandomPlayer()
			loc = battleGetLocation(id)
			ox = battleGetX(target) - battleGetX(id)
			battleSetOx(id, ox)
			--dy = battleGetY(target) - battleGetY(id)
			--myIndex = battleGetEntityIndex(id)
			--enemyIndex = battleGetEntityIndex(target)
			--battleMoveEntity(myIndex, enemyIndex)
			dy = 0
			old_y = battleGetY(id)
			local my_y = battleGetY(target)
			battleSetY(id, my_y+1)
			battleResortEntity(id)
			loadPlayDestroy("fall.ogg")
			stage = stage + 1
		end
	elseif (stage == STAGE_FALLING) then
		oy = fixadd(oy, fixmul(SPEED, step))
		battleSetOy(id, fixtoi(oy))
		if (fixtoi(oy) >= dy) then
			oy = dy
			loadPlayDestroy("Thud.ogg")
			stage = stage + 1
		end
	elseif (stage == STAGE_POUNDING) then
		delay = delay + step
		if (delay > 1000) then
			delay = 0
			stage = stage + 1
		end
	elseif (stage == STAGE_BACKUP) then
		oy = fixsub(oy, fixmul(fixmul(SPEED, 2), step))
		battleSetOy(id, fixtoi(oy))
		if (fixtoi(oy) < MAX_HEIGHT) then
			ox = 0
			battleSetOx(id, ox)
			stage = stage + 1
			--battleMoveEntity(enemyIndex, myIndex)
			battleSetY(id, old_y)
			battleResortEntity(id)
		end
	elseif (stage == STAGE_BACKDOWN) then
		oy = fixadd(oy, fixmul(fixmul(SPEED, 2), step))
		battleSetOy(id, fixtoi(oy))
		if (fixtoi(oy) > 0) then
			oy = 0
			battleSetOy(id, 0)
			stage = 0
			return COMBAT_ATTACKING, 1, target
		end
	end

	return COMBAT_BUSY
end

function die()
end

SHADOW_MAX_RX = 15
SHADOW_MAX_RY = 8

function pre_draw()
	if (stage >= STAGE_JUMPING and stage <= STAGE_BACKDOWN) then
		local y
		if (stage == STAGE_JUMPING or stage == STAGE_BACKDOWN) then
			y = start_y
		else
			y = battleGetY(target)
		end
		local p = fixsub(1, fixdiv(oy, MAX_HEIGHT))
		local rx = fixmul(SHADOW_MAX_RX, p)
		local ry = fixmul(SHADOW_MAX_RY, p)
		fillEllipse(start_x+ox, y, fixtoi(rx), fixtoi(ry), 0, 0, 0, 160)
	end
end


