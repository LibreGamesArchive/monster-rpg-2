MOVE_NONE = 1
MOVE_WANDER = 2
MOVE_LINE = 3
MOVE_FOLLOW = 4
MOVE_PUSHED = 5 -- for push boxes


--[[
Scripting:
each Object has:
	current_scripted_event (number)
	scripted_events        (list)

scripted_events has:
	event_type

Event types:
	EVENT_WALK		dest_x (number), dest_y (number)
	EVENT_SPEAK		text (string)
	EVENT_REST		delay (number)
]]--


EVENT_WALK = 1
EVENT_SPEAK = 2
EVENT_REST = 3
EVENT_LOOK = 4
EVENT_WAIT_FOR_SPEECH = 5
EVENT_SYNC = 6
EVENT_DESCRIPTIFY = 7
EVENT_GESTURE = 8
EVENT_SPAWN = 9
EVENT_CUSTOM = 10
EVENT_BATTLE = 11
EVENT_WAIT_FOR_BATTLE = 12


-- Enemies for floating fortress (so it's managed in one place)
fort_enemies = {}
fort_enemies[0] = "2Toads"
fort_enemies[1] = "1Gator"
fort_enemies[2] = "3Fungi"
fort_enemies[3] = "2Flux1Fungus"
fort_enemies[4] = "2Droplets1Gator"
fort_enemies[5] = "1Octopus"
fort_enemies[6] = "4Medusas"

Object = {
	move_type = MOVE_NONE,
	id = nil,
	startx = 0,
	starty = 0,
	anim_set = nil,
	rest = 3000,
	maxdist = 8,
	solid = true,
	resting = false,
	rest_count = 0,
	current = 1,
}



function Object:new(o)
	o = o or {}
	setmetatable(o, self)
	self.__index = self
	o.x = o.x or 0
	o.y = o.y or 0
	o.startx = o.x
	o.starty = o.y
	o.rest = o.rest or Object.rest
	o.next_move = 0
	o.maxdist = o.maxdist or Object.maxdist
	o.current_scripted_event = o.current_scripted_event or 1
	o.id = o.id or addObject();
	-- stuff for WALK_LINE
	if (o.move_type == MOVE_LINE) then
		o.resting = Object.resting
		o.rest_count = Object.rest_count
		o.current = Object.current
		o.targetx = o.x
		o.targety = o.y
	end
	-- end
	if (o.solid == nil) then
		o.solid = true
	end
	setObjectSolid(o.id, o.solid)
	if (o.high) then
		setObjectHigh(o.id, true)
	end
	setObjectPosition(o.id, o.x, o.y)
	if (not (o.anim_set == nil)) then
		setObjectAnimationSet(o.id, o.anim_set)
	end
	if (not (o.id == 0)) then
		setObjectInputToScriptControlled(o.id);
	end
	o.move_type = o.move_type or MOVE_NONE
	if (o.move_type == MOVE_WANDER) then
		o:wander()
	end
	if (o.person == true) then
		setObjectIsPerson(o.id, true)
	end
	if (not (o.direction == nil)) then
		setObjectDirection(o.id, o.direction)
	end
	return o
end



function Object:rand_wander_direction()
	local r
	local ox, oy = getObjectPosition(self.id)
	local x, y
	local i = 0

	repeat
		r = randint(4) + 1;
		x = ox
		y = oy
		if (r == DIRECTION_NORTH) then
			y = y - 1
		elseif (r == DIRECTION_EAST) then
			x = x + 1
		elseif (r == DIRECTION_SOUTH) then
			y = y + 1
		elseif (r == DIRECTION_WEST) then
			x = x - 1
		end
		i = i + 1
	until ((not areaIsSolid(self.id, x, y)) or i > 10)

	if (i > 10) then
		return -1
	end

	return r
end



function Object:wander()
	local x, y = getObjectPosition(self.id)
	local dx = x - self.startx
	local dy = y - self.starty

	if (math.abs(dx) > self.maxdist or
			math.abs(dy) > 
			self.maxdist) then
		local l = false
		local r = false
		local u = false
		local d = false
		local b1 = false
		local b2 = false
		local ox, oy = 0, 0
		if (dx < -self.maxdist) then
			r = true
			ox = 1
		elseif (dx > self.maxdist) then
			l =  true
			ox = -1
		elseif (dy < -self.maxdist) then
			d = true
			oy = 1
		elseif (dy > self.maxdist) then
			u = true
			oy = -1
		end
		if (not areaIsSolid(self.id, x+ox, y+oy)) then
			setObjectInput(self.id, l, r, u, d, b1, b2)
			return
		end
	end


	direction = self:rand_wander_direction()

	if (direction == DIRECTION_NORTH) then
		setObjectInput(self.id, false, false, true, false, false, false)
	elseif (direction == DIRECTION_EAST) then
		setObjectInput(self.id, false, true, false, false, false, false)
	elseif (direction == DIRECTION_WEST) then
		setObjectInput(self.id, true, false, false, false, false, false)
	elseif (direction == DIRECTION_SOUTH) then
		setObjectInput(self.id, false, false, false, true, false, false)
	end
end


function Object:walk_towards(dx, dy)
	local x, y = getObjectPosition(self.id)
	local l, r, u, d = false, false, false, false

	if (not (x == dx) and not (y == dy)) then
		r = randint(2)
		if (r == 1) then
			if (x > dx) then
				self.targetx = x - 1
				self.targety = y
				l = true
			else
				self.targetx = x + 1
				self.targety = y
				r = true
			end
		else
			if (y > dy) then
				self.targety = y - 1
				self.targetx = x
				u = true
			else
				self.targety = y + 1
				self.targetx = x
				d = true
			end
		end
	elseif (not (x == dx)) then
		if (x > dx) then
			self.targetx = x - 1
			self.targety = y
			l = true
		else
			self.targetx = x + 1
			self.targety = y
			r = true
		end
	else
		if (y > dy) then
			self.targety = y - 1
			self.targetx = x
			u = true
		else
			self.targety = y + 1
			self.targetx = x
			d = true
		end
	end

	if (areaIsSolid(self.id, self.targetx, self.targety)) then
		self.targetx, self.targety = x, y
		return
	end

	setObjectInput(self.id, l, r, u, d, false, false)
end


-- dx1, dx2, dy1, dy2, pause, dir1, dir2
function Object:move_line(step)
	local x, y = getObjectPosition(self.id)

	if (self.resting) then
		self.rest_count = self.rest_count + step
		if (self.rest_count > self.pause) then
			self.rest_count = 0
			self.resting = false
			if (self.current == 1) then
				self.current = 2
			else
				self.current = 1
			end
		else
			return
		end
	end

	local dx, dy, dir

	if (self.current == 1) then
		dx = self.dx2
		dy = self.dy2
		dir = self.dir2
	else
		dx = self.dx1
		dy = self.dy1
		dir = self.dir1
	end

	if (self.targetx == x and self.targety == y) then
		if (self.targetx == dx and self.targety == dy) then
			setObjectDirection(self.id, dir)
			self.resting = true
		else
			self:walk_towards(dx, dy)
		end
	else
		setObjectInput(self.id, false, false, false, false, false, false)
	end
end


function Object:move_follow(step)
	myx, myy = getObjectPosition(self.id)
	fx, fy = getObjectPosition(self.follow.id)

	if (myx < fx) then
		setObjectInput(self.id, false, true, false, false, false, false)
	elseif (myx > fx) then
		setObjectInput(self.id, true, false, false, false, false, false)
	elseif (myy < fy) then
		setObjectInput(self.id, false, false, false, true, false, false)
	elseif (myy > fy) then
		setObjectInput(self.id, false, false, true, false, false, false)
	end
end

function Object:move_pushed(step)
	if (self.pushed) then
		self.pushed = false
		setObjectInput(self.id, self.pushl, self.pushr, self.pushu, self.pushd, false, false)
	end
end


function Object:move(step)
	if (isSpeechDialogActive()) then
		return
	end

	self.next_move = self.next_move + step
	if (self.next_move > self.rest) then
		self.next_move = self.next_move - self.rest;
	else
		setObjectInput(self.id, false, false, false, false, false, false)
		return
	end

	if (self.move_type == MOVE_WANDER) then
		self:wander()
	elseif (self.move_type == MOVE_LINE) then
		self:move_line(step)
	elseif (self.move_type == MOVE_FOLLOW) then
		self:move_follow(step)
	elseif (self.move_type == MOVE_PUSHED) then
		self:move_pushed(step)
	end
end



function Object:scripted_move(event)
	x, y = getObjectPosition(self.id)

	dx = event.dest_x
	dy = event.dest_y

	if ((x == dx) and (y == dy)) then
		setObjectInput(self.id, false, false, false, false, false, false)
		return true;
	end

	l = false
	r = false
	u = false
	d = false

	if (x < dx) then
		r = true
	elseif (x > dx) then
		l = true
	elseif (y < dy) then
		d = true
	elseif (y > dy) then
		u = true
	end

	setObjectInput(self.id, l, r, u, d, false, false)

	

--[[

	if (event.current_step_x == nil or
			((event.current_step_x == x 
			and event.current_step_y == y)
			)) then

		if (event.current_step_x == nil) then
			event.current_step_x = x
			event.current_step_y = y
		end

		l = false
		r = false
		u = false
		d = false

		if (x < dx) then
			r = true
			event.current_step_x = x + 1
		elseif (x > dx) then
			l = true
			event.current_step_x = x - 1
		elseif (y < dy) then
			d = true
			event.current_step_y = y + 1
		else
			u = true
			event.current_step_y = y - 1
		end

		setObjectInput(self.id, l, r, u, d, false, false)

		self.moved = true

	elseif (self.moved == true) then
		setObjectInput(self.id, false, false, false, false, false, false)
		self.moved = false
	end
--]]


	return false;
end



function Object:update(step)
	if (self.current_scripted_event <= #self.scripted_events) then
		local event = self.scripted_events[self.current_scripted_event];
		-- Object has scripted events left
		-- Walk
		if (event.event_type == EVENT_WALK) then
			if (not event.dest_set) then
				setObjectDest(self.id, event.dest_x, event.dest_y)
				event.dest_set = true
			end
			if (self:scripted_move(event) == true) then
				self.current_scripted_event
					= self.current_scripted_event + 1
			end
		-- Speak
		elseif (event.event_type == EVENT_SPEAK) then
			local top = event.top or false
			local bottom = event.bottom or false
			doDialogue(event.text, false, top, bottom)
			self.current_scripted_event
				= self.current_scripted_event + 1
		-- Rest
		elseif (event.event_type == EVENT_REST) then
			event.delay = event.delay - step
			if (event.delay <= 0) then
				self.current_scripted_event
					= self.current_scripted_event + 1
			end
		elseif (event.event_type == EVENT_LOOK) then
			setObjectDirection(self.id, event.direction)
			self.current_scripted_event
				= self.current_scripted_event + 1
		elseif (event.event_type == EVENT_WAIT_FOR_SPEECH) then
			if (not isSpeechDialogActive()) then
				self.current_scripted_event =
					self.current_scripted_event + 1
			end
		elseif (event.event_type == EVENT_SYNC) then
			if (event.number < event.who.current_scripted_event) then
				self.current_scripted_event =
					self.current_scripted_event + 1
			end
		elseif (event.event_type == EVENT_DESCRIPTIFY) then
			descriptifyPlayer();
			self.current_scripted_event =
				self.current_scripted_event + 1
		elseif (event.event_type == EVENT_GESTURE) then
			setObjectSubAnimation(self.id, event.name)
			self.current_scripted_event =
				self.current_scripted_event + 1
		elseif (event.event_type == EVENT_SPAWN) then
			tmp = Object:new(event.what)
			self.current_scripted_event =
				self.current_scripted_event + 1
		elseif (event.event_type == EVENT_CUSTOM) then
			event.callback(self, event)
			self.current_scripted_event =
				self.current_scripted_event + 1
		elseif (event.event_type == EVENT_BATTLE) then
			if (event.must_win == false) then
				if (event.can_run == false) then
					startBattle(event.id, false, false)
				else
					startBattle(event.id, false, true)
				end
			else
				if (event.can_run == false) then
					startBattle(event.id, true, false)
				else
					startBattle(event.id, true, true)
				end
			end
			self.current_scripted_event =
				self.current_scripted_event + 1
		elseif (event.event_type == EVENT_WAIT_FOR_BATTLE) then
			if (not inBattle()) then
				self.current_scripted_event =
					self.current_scripted_event + 1
			end
		end
	else
		self:move(step)
	end

end

Chest = {}

function Chest:activate()
	if (not self.open) then
		loadPlayDestroy("chest.ogg");
		if (self.battle) then
			setMilestone(self.milestone, true)
			self.open = true
			if (not (self.anim_set == nil)) then
				setObjectSubAnimation(self.id, "open")
			end
			startBattle(self.battle, true)
		elseif (self.itemtype == ITEM_GOLD) then
			if (not (self.anim_set == nil)) then
				setObjectSubAnimation(self.id, "open")
			end
			giveGold(self.quantity)
			setMilestone(self.milestone, true)
			msg = _t("Found ") .. self.quantity .. _t(" gold") .. "!\n"
			doDialogue(msg)
			self.open = true
		else
			slot = findUsedInventorySlot(self.index)
			if (slot >= 0) then
				local index, quantity = getInventory(slot)
				if (99-quantity >= self.quantity) then
					self.open = true
					if (not (self.anim_set == nil)) then
						setObjectSubAnimation(self.id, "open")
					end
					setInventory(slot, self.index, self.quantity+quantity)
					setMilestone(self.milestone, true);
					if (self.quantity > 1) then
						doDialogue(_t("Found ") .. self.quantity .." " .. getItemIcon(self.index) .. _t(getItemName(self.index) .. "s") .. "\n")
					else
						if (startsWithVowel(getItemName(self.index))) then
							doDialogue(_t("Found an ") .. getItemIcon(self.index) .. _t(getItemName(self.index)) .. "\n")
						else
							doDialogue(_t("Found a ") .. getItemIcon(self.index) .. _t(getItemName(self.index)) .. "\n")
						end
					end
					return
				end
			end

			slot = findEmptyInventorySlot()
			if (slot < 0) then
				doDialogue("Inventory is full...\n")
			else
				self.open = true
				if (not (self.anim_set == nil)) then
					setObjectSubAnimation(self.id, "open")
				end
				setInventory(slot, self.index, self.quantity)
				setMilestone(self.milestone, true);
				if (self.quantity > 1) then
					doDialogue(_t("Found ") .. self.quantity .." " .. getItemIcon(self.index) .. _t(getItemName(self.index) .. "s") .. "\n")
				else
					if (startsWithVowel(getItemName(self.index))) then
						doDialogue(_t("Found an ") .. getItemIcon(self.index) .. _t(getItemName(self.index)) .. "\n")
					else
						doDialogue(_t("Found a ") .. getItemIcon(self.index) .. _t(getItemName(self.index)) .. "\n")
					end
				end
			end
		end
	end
end

function Chest:new(o)
	o = Object:new(o)
	o.activate = Chest.activate
	o.open = getMilestone(o.milestone)
	if (o.open and not (o.anim_set == nil)) then
		setObjectSubAnimation(o.id, "open")
	end
	o.quantity = o.quantity or 1
	return o
end

Portal = {}


function Portal:new(o)
	o = o or {}
	setmetatable(o, self)
	self.__index = self
	o.x = o.x or 0
	o.y = o.y or 0
	o.can_go = false
	o.width = o.width or 1 
	o.height = o.height or 1
	return o
end


function Portal:colliding()
	px, py = getObjectPosition(0)
	if ((px >= self.x) and (px < (self.x+self.width)) and (py >= self.y) and (py < (self.y+self.height))) then
		return true
	else
		return false
	end
end



function Portal:update()
	if (not self.can_go) then
		if (not self:colliding()) then
			self.can_go = true
		end
	else
		if (self:colliding()) then
			self.go_time = true
		end
	end

	-- If the player's destination is not this portal but they
	-- have stepped on it, do not activate it
	if (self.go_time) then
		-- FIXME: will this work on PC?
		local dx, dy
		dx, dy = get_player_dest()
		if (not (dx == self.x) or not (dy == self.y)) then
			local i
			-- FIXME: layers hardcoded
			for i=1,4 do
				local n = getTileLayer(self.x, self.y, i-1)
				n = areaTileToTilemap(n)
				-- FIXME: stairs tiles hardcoded
				if (n == 98 or n == 162 or n == 154 or n == 378 or n == 1014 or n == 1046) then
					self.go_time = false
					return false
				end
			end
		end
	end

	return self.go_time == true
end


-- check_battle:
-- Exceptions is a table like this:
-- {
--    { x1, y1 },
--    { x2, y2 },
-- }
-- Where x, y is a tile not to do battle on

_player_x = -1
_player_y = -1

_last_battle_x = -1
_last_battle_y = -1
_last_battle_name = ""

function check_battle(chances, enemies, exceptions)
	if (ChangedAreasJustNow == true) then
		return
	end
	if (_player_x < 0) then
		-- Don't battle before move
		_player_x, _player_y = getObjectPosition(0)
		return
	end
	local px, py = getObjectPosition(0)
	if (_last_battle_x == -1) then
		-- do nothing
	elseif (math.floor(math.abs(px-_last_battle_x) + math.abs(py-_last_battle_y)) <= 1) then
		return
	end
	if (not (px == _player_x) or not (py == _player_y)) then
		_player_x = px
		_player_y = py
		for i=1,#exceptions do
			if ((px == exceptions[i][1]) and (py == exceptions[i][2])) then
				return
			end
		end
		chances = chances + extra_battle_chances()
		if (randint(chances) == 0) then
			if (#enemies == 0) then
				enemy = enemies[0]
			else
				repeat
					enemy = enemies[randint(#enemies+1)]
				until (not (enemy == _last_battle_name))
			end
			_last_battle_x = px
			_last_battle_y = py
			_last_battle_name = enemy
			startBattle(enemy, true)
		end
	end
end


Door = {}


function Door:open(mute)
	if (self.isOpen) then
		return
	end
	if (not mute) then
		loadPlayDestroy("door.ogg")
	end
	setObjectSubAnimation(self.id, "open")
	setObjectSolid(self.id, false)
	self.isOpen = true
end

function Door:close()
	if (not self.isOpen) then
		return
	end
	loadPlayDestroy("door.ogg")
	setObjectSubAnimation(self.id, "closed")
	setObjectSolid(self.id, true)
	self.isOpen = false
end

function Door:new(o)
	o = Object:new(o)
	setObjectSubAnimation(o.id, "closed")
	o.open = Door.open
	o.close = Door.close
	setObjectSolid(o.id, true)
	return o
end


-- shoulda done this sooner...
function change_areas(area, x, y, dir, trans_in)
	ChangedAreasJustNow = true -- stop battle from happening immediately after entering a room (even a room with no battles defined)
	stopObject(0)
	if (not (area == "moon_landing" or (area == "fort_start" and not (string.sub(getAreaName(), 0, 4) == "fort")))) then
		drawArea()
		drawBufferToScreen()
		transitionOut()
	end
	if (area == "portal" and not getMilestone(MS_DONE_CREDITS)) then
		setMilestone(MS_DONE_CREDITS, true)
		credits()
	end
	setObjectPosition(0, x, y)
	startArea(area)
	setObjectDirection(0, dir)
	clearBuffer()
	updateArea()
	-- hack for Archery game
	if (getBreakMainLoop()) then return end
	drawArea()
	drawBufferToScreen()
	if (trans_in == nil) then
		dpad_off()
		transitionIn()
		dpad_on()
	end
end

-- get direction to face player
function player_dir(person)
	px, py = getObjectPosition(0)
	if (person.move_type == MOVE_WANDER) then
		person.x, person.y = getObjectPosition(person.id)
	end
	if (px == person.x and py < person.y) then
		return DIRECTION_NORTH
	elseif (px > person.x and py == person.y) then
		return DIRECTION_EAST
	elseif (px == person.x and py > person.y) then
		return DIRECTION_SOUTH
	elseif (px < person.x and py == person.y) then
		return DIRECTION_WEST
	end
end


function add_downward_light(x, y, topw, bottomw, length, r, g, b, a)
	addLight(x-bottomw/2, y, DIRECTION_SOUTH, topw, bottomw, length, r, g, b, a)
end

function smartDialogue(o)
	local i = #o-1

	while true do
		if (getMilestone(o[i])) then
			i = i + 1
			break
		end

		i = i - 2
		if (i < 1) then
			i = 1
			break
		end
	end

	doDialogue(o[i], true)
end

