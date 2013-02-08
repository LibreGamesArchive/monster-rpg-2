if (getMilestone(MS_KILLED_GOLEMS)) then
	music = "Flowey.ogg"
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
			local _d = getObjectDirection(fat_old_guy.id)
			setObjectDirection(fat_old_guy.id, player_dir(fat_old_guy))
			doDialogue("I used to be a warrior when I was young, but I can't fight anymore...\nI am blessed with a son, who may continue with my legacy.\n", true)
			setObjectDirection(fat_old_guy.id, _d)
		elseif (activated == babies_mother.id) then
			local _d = getObjectDirection(babies_mother.id)
			setObjectDirection(babies_mother.id, player_dir(babies_mother))
			doDialogue("We led such a peaceful life until recently... The peace will return, one day.\n", true)
			setObjectDirection(babies_mother.id, _d)
		elseif (activated == Kamwerere.id) then
			doDialogue("I must watch over my brother, oh my!\n", true)
		elseif (activated == toddler.id) then
			local _d = getObjectDirection(toddler.id)
			setObjectDirection(toddler.id, player_dir(toddler))
			doDialogue("Goo goo ga ga...\n", true)
			setObjectDirection(toddler.id, _d)
		elseif (activated == Trent.id) then
			local _d = getObjectDirection(Trent.id)
			setObjectDirection(Trent.id, player_dir(Trent))
			doDialogue("Trying to sleep here...\n", true)
			setObjectDirection(Trent.id, _d)
		elseif (activated == Mom.id) then
			local _d = getObjectDirection(Mom.id)
			setObjectDirection(Mom.id, player_dir(Mom))
			doDialogue("Hey, what 'ya doing?\n", true)
			setObjectDirection(Mom.id, _d)
		elseif (activated == Darryl.id) then
			local _d = getObjectDirection(Darryl.id)
			setObjectDirection(Darryl.id, player_dir(Darryl))
			doDialogue("I need to get back to work, but it's so dangerous out there...\nThank goodness for you guys, this one safe place could have easily fallen.\n", true)
			setObjectDirection(Darryl.id, _d)
		end
	end
end

function collide(id1, id2)
end

