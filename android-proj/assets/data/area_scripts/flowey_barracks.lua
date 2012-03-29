if (getMilestone(MS_KILLED_GOLEMS)) then
	music = "Flowey.caf"
end

function start()
	up = Portal:new{x=11, y=7}
	out = Portal:new{x=6, y=14}
	stuff1 = Object:new{x=1, y=3}
	stuff2 = Object:new{x=2, y=3}
end

function stop()
end

function update(step)
	if (up:update()) then
		change_areas("flowey_barracks2", 11, 7, DIRECTION_WEST)
	elseif (out:update()) then
		change_areas("flowey", 13, 28, DIRECTION_SOUTH)
	end
end

function activate(activator, activated)
	if (activated == stuff1.id or activated == stuff2.id) then
		doDialogue("Eny: We better not take that... the guards need it.\n")
	end
end

function collide(id1, id2)
end

