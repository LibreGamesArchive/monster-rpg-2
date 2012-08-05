if (getMilestone(MS_KILLED_GOLEMS)) then
	music = "Flowey.ogg"
end

function look()
	setObjectDirection(librarian.id, DIRECTION_EAST)
end

function look_back()
	setObjectDirection(librarian.id, DIRECTION_WEST)
end

function done()
	descriptifyPlayer()
	removeObject(rios.id)
	setMilestone(MS_RIOS_TALKED_TO_LIBRARIAN, true)
end

function start()
	up = Portal:new{x=7, y=3}

	if (not getMilestone(MS_RIOS_LEFT)) then
		librarian = Object:new{x=2, y=6, anim_set="sensei", person=true}
		setObjectDirection(librarian.id, DIRECTION_WEST)

		if (not getMilestone(MS_RIOS_TALKED_TO_LIBRARIAN)) then
			scriptifyPlayer()
			px, py = getObjectPosition(0)
			rios = Object:new{x=px, y=(py+1), anim_set="Rios", person=true}
			setObjectSolid(rios.id, false)
			rios.scripted_events = {
				{ event_type=EVENT_WALK, dest_x=7, dest_y=6 },
				{ event_type=EVENT_WALK, dest_x=3, dest_y=6 },
				{ event_type=EVENT_CUSTOM, callback=look },
				{ event_type=EVENT_SPEAK, text="Rios: I've come from Seaside where the library has been destroyed. I'm here to rewrite our history.\nLibrarian: Oh what a tragic event! Please, stay and commence with your work.\nRios: I will come back, first I must help deal with the invaders on the beach.\nLibrarian: Be safe my friend, I will see you soon.\n", top=true },
				{ event_type=EVENT_WAIT_FOR_SPEECH },
				{ event_type=EVENT_CUSTOM, callback=look_back },
				{ event_type=EVENT_WALK, dest_x=7, dest_y=6 },
				{ event_type=EVENT_WALK, dest_x=7, dest_y=py },
				{ event_type=EVENT_CUSTOM, callback=done },
			}
		end
	else
		librarian = Object:new{x=1, y=5, anim_set="sensei", person=true}
		setObjectDirection(librarian.id, DIRECTION_SOUTH)
		if (not getMilestone(MS_BEAT_TODE)) then
			rios = Object:new{x=2, y=6, anim_set="Rios", person=true}
			setObjectDirection(rios.id, DIRECTION_WEST)
		end
	end
end

function stop()
end

function update(step)
	if (up:update()) then
		change_areas("lib", 7, 3, DIRECTION_SOUTH)
	end

	if (not getMilestone(MS_RIOS_TALKED_TO_LIBRARIAN)) then
		rios:update(step)
	end
end

function activate(activator, activated)
	if (getMilestone(MS_RIOS_LEFT)) then
		if (activated == librarian.id) then
			local _d = getObjectDirection(librarian.id)
			setObjectDirection(librarian.id, player_dir(librarian))
			if (getMilestone(MS_BEAT_TODE)) then
				doDialogue("Welcome back!\n", true)
			else
				doDialogue("I'm making copies of everything so there is one for here and one for Seaside...\n", true)
			end
			setObjectDirection(librarian.id, _d)
		elseif (rios and activated == rios.id) then
			local _d = getObjectDirection(rios.id)
			setObjectDirection(rios.id, player_dir(rios))
			if (getMilestone(MS_BEAT_GIRL_DRAGON)) then
				doDialogue("Rios: The Staff seeks a soul to bind to...\nIt knows it is in danger here.\nDid you know that we once had communication with moon-folk?\n", true)
			else
				doDialogue("Rios: This may take a while, but it has to be done.\nRios: Be very careful in your quest!\n", true)
			end
			setObjectDirection(rios.id, _d)
		end
	else
		if (activated == librarian.id) then
			local _d = getObjectDirection(librarian.id)
			setObjectDirection(librarian.id, player_dir(librarian))
			doDialogue("Take care and come back soon!\n", true)
			setObjectDirection(librarian.id, _d)
		end
	end
end

function collide(id1, id2)
end

