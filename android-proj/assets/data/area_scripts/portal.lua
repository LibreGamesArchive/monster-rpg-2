music = "forest.caf"

function start()
	portal = Object:new{x=8, y=8, anim_set="Portal", solid=false}
	setObjectLow(portal.id, true)
	out = Portal:new{x=0, y=8, width=1, height=2}
end

function stop()
end

function update(step)
	local px, py = getObjectPosition(0)
	if (px == 9 and py == 7) then
		stopObject(0)
		setMilestone(MS_ON_MOON, true)
		loadPlayDestroy("Portal.ogg")
		change_areas("darkside", 10, 9, DIRECTION_SOUTH)
	end

	if (out:update()) then
		doMap("portal", "map")
	end
end

function activate(activator, activated)
end

function collide(id1, id2)
end

