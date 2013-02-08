hp = 150
attack = 50
defense = 45
speed = 36
mdefense = 0
luck = 10

strength = ELEMENT_NONE
weakness = ELEMENT_NONE

stage = 0

STAGE_INIT     = 0
STAGE_JUMPING  = 1
STAGE_FALLING  = 2
STAGE_CHOMPING = 3
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
	preloadSFX("chomp.ogg")
end

function get_action(step)
	if (stage == STAGE_INIT) then
		loadPlayDestroy("jump.ogg")
		stage = stage + 1
		delay = 0
		start_x = battleGetX(id)
		start_y = battleGetY(id)
	elseif (stage == STAGE_JUMPING) then
		oy = oy - (SPEED * step)
		battleSetOy(id, oy)
		if (oy < MAX_HEIGHT) then
			target = getRandomPlayer()
			loc = battleGetLocation(id)
			ox = battleGetX(target) - battleGetX(id)
			battleSetOx(id, ox)
			if (loc == LOCATION_LEFT) then
				angle = -1 * (math.pi / 2)
			else
				angle = math.pi / 2
			end
			battleSetAngle(id, angle)
			dy = 0
			old_y = battleGetY(id)
			local my_y = battleGetY(target)
			battleSetY(id, my_y+1)
			battleResortEntity(id)
			loadPlayDestroy("fall.ogg")
			stage = stage + 1
		end
	elseif (stage == STAGE_FALLING) then
		oy = oy + (SPEED * step)
		battleSetOy(id, oy)
		if (oy >= dy) then
			oy = dy
			loadPlayDestroy("chomp.ogg")
			stage = stage + 1
		end
	elseif (stage == STAGE_CHOMPING) then
		delay = delay + step
		if (delay > 1000) then
			delay = 0
			stage = stage + 1
		end
	elseif (stage == STAGE_BACKUP) then
		oy = oy - ((SPEED * 2) * step)
		battleSetOy(id, oy)
		if (oy < MAX_HEIGHT) then
			ox = 0
			battleSetOx(id, ox)
			stage = stage + 1
			angle = 0
			battleSetAngle(id, angle)
			--battleMoveEntity(enemyIndex, myIndex)
			battleSetY(id, old_y)
			battleResortEntity(id)
		end
	elseif (stage == STAGE_BACKDOWN) then
		oy = oy + ((SPEED * 2) * step)
		battleSetOy(id, oy)
		if (oy > 0) then
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

SHADOW_MAX_RX = 20
SHADOW_MAX_RY = 10

function pre_draw()
	if (stage >= STAGE_JUMPING and stage <= STAGE_BACKDOWN) then
		local y
		if (stage == STAGE_JUMPING or stage == STAGE_BACKDOWN) then
			y = start_y
		else
			--y = start_y + dy
			y = battleGetY(target)
		end
		local p = 1 - (oy / MAX_HEIGHT)
		local rx = SHADOW_MAX_RX * p
		local ry = SHADOW_MAX_RY * p
		fillEllipse(start_x+ox, y, rx, ry, 0, 0, 0, 160)
	end
end


