if (getMilestone(MS_KILLED_GOLEMS)) then
	music = "Flowey.caf"
end

function start()
	add_downward_light(41, 28, 8, 32, 48, 255, 255, 150, 60) 
	add_downward_light(73, 28, 8, 32, 48, 255, 255, 150, 60) 
	add_downward_light(105, 28, 8, 32, 48, 255, 255, 150, 60) 
	add_downward_light(233, 28, 8, 32, 48, 255, 255, 150, 60) 
	add_downward_light(265, 28, 8, 32, 48, 255, 255, 150, 60) 
	add_downward_light(297, 28, 8, 32, 48, 255, 255, 150, 60)

	down = Portal:new{x=9, y=3}

	if (not getMilestone(MS_BEAT_TODE)) then
		fat_old_guy = Object:new{x=2, y=9, anim_set="fat_old_guy", person=true}
		setObjectDirection(fat_old_guy.id, DIRECTION_EAST)
		babies_mother = Object:new{x=4, y=8, anim_set="babies_mother", person=true}
		setObjectDirection(babies_mother.id, DIRECTION_SOUTH)
		toddler = Object:new{x=4, y=6, anim_set="toddler", person=true, move_type=MOVE_WANDER}
		Kamwerere = Object:new{x=3, y=6, anim_set="Kamwerere", person=true, move_type=MOVE_FOLLOW, follow=toddler}

		Trent = Object:new{x=14, y=3, anim_set="Trent", person=true}
		setObjectDirection(Trent.id, DIRECTION_SOUTH)
		Mom = Object:new{x=18, y=9, anim_set="Mom", person=true}
		setObjectDirection(Mom.id, DIRECTION_WEST)
		Darryl = Object:new{x=14, y=9, anim_set="Darryl", person=true}
		setObjectDirection(Darryl.id, DIRECTION_EAST)
	end
end

function stop()
end

function update(step)
	down:update()

	if (down.go_time) then
		change_areas("inn1", 9, 3, DIRECTION_EAST)
	end

	toddler:move(step)
	Kamwerere:move(step)
end

function activate(activator, activated)
	if (not getMilestone(MS_BEAT_TODE)) then
		if (activated == fat_old_guy.id) then
			doDialogue("I used to be a warrior when I was young, but I can't fight anymore...\nI am blessed with a son, who may continue with my legacy.\n")
		elseif (activated == babies_mother.id) then
			doDialogue("We led such a peaceful life until recently... The peace will return, one day.\n")
		elseif (activated == Kamwerere.id) then
			doDialogue("I must watch over my brother, oh my!\n")
		elseif (activated == toddler.id) then
			doDialogue("Goo goo ga ga...\n")
		elseif (activated == Trent.id) then
			doDialogue("Trying to sleep here...\n")
		elseif (activated == Mom.id) then
			doDialogue("Hey, what 'ya doing?\n")
		elseif (activated == Darryl.id) then
			doDialogue("I need to get back to work, but it's so dangerous out there...\nThank goodness for you guys, this one safe place could have easily fallen.\n")
		end
	end
end

function collide(id1, id2)
end

