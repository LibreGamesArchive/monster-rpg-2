music = "shyzu.ogg"

function start()
	out = Portal:new{x=2, y=6}
	male = Object:new{x=1, y=4, anim_set="shyzu_male", person=true, dx1=1, dy1=4, dir1=DIRECTION_WEST, dx2=4, dy2=4, dir2=DIRECTION_EAST, move_type=MOVE_LINE, pause=3000, rest=0}
end

function stop()
end

function update(step)
	if (out:update()) then
		change_areas("shyzu", 21, 18, DIRECTION_SOUTH)
	end

	male:move(step)
end

function activate(activator, activated)
	if (activated == male.id) then
		doDialogue("Male: If one wants a wife, he must brave the dark side... Alas, I am too cowardly.\n")
	end
end

function collide(id1, id2)
end

