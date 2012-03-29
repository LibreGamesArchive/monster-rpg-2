if (getMilestone(MS_KILLED_GOLEMS)) then
	music = "Flowey.caf"
end

function start()
	add_downward_light(41, 28, 8, 32, 48, 255, 255, 150, 60); 
	add_downward_light(89, 28, 8, 32, 48, 255, 255, 150, 60); 
	add_downward_light(249, 28, 8, 32, 48, 255, 255, 150, 60); 
	add_downward_light(297, 28, 8, 32, 48, 255, 255, 150, 60); 

	up = Portal:new{x=11, y=3}
	out = Portal:new{x=10, y=14}

	innkeeper = Object:new{x=19, y=7, anim_set="mokkan", person=true}
	setObjectDirection(innkeeper.id, DIRECTION_WEST)
	fake = Object:new{x=18, y=7}
end

function stop()
end

function update(step)
	up:update()
	out:update()

	if (up.go_time) then
		change_areas("inn1", 11, 3, DIRECTION_WEST)
	elseif (out.go_time) then
		change_areas("flowey", 8, 12, DIRECTION_SOUTH)
	end
end
	
function activate_shop()
	doDialogue("Innkeeper: There is an empty room on the second floor. Please be our guests...\n")
end

function activate(activator, activated)
	if (activated == innkeeper.id or activated == fake.id) then
		activate_shop()
	end
end

function activate_any(tx, ty)
	local px, py = getObjectPosition(0)
	if (px == 17 and py == 7 and tx == 19 and ty == 7) then
		stopObject(0)
		setObjectDirection(0, DIRECTION_EAST);
		activate_shop()
		return true
	end
	return false
end

function collide(id1, id2)
end

