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
	
	books = {}
	books[1] = Object:new{x=1, y=3}
	books[2] = Object:new{x=3, y=3}
	books[3] = Object:new{x=5, y=3}
	books[4] = Object:new{x=8, y=3}
	books[5] = Object:new{x=10, y=3}
	books[6] = Object:new{x=12, y=3}
	books[7] = Object:new{x=8, y=6}
	books[8] = Object:new{x=10, y=6}
	books[9] = Object:new{x=12, y=6}
	
	books2 = {}
	books2[1] = Object:new{x=1+1, y=3}
	books2[2] = Object:new{x=3+1, y=3}
	books2[3] = Object:new{x=5+1, y=3}
	books2[4] = Object:new{x=8+1, y=3}
	books2[5] = Object:new{x=10+1, y=3}
	books2[6] = Object:new{x=12+1, y=3}
	books2[7] = Object:new{x=8+1, y=6}
	books2[8] = Object:new{x=10+1, y=6}
	books2[9] = Object:new{x=12+1, y=6}
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

texts = {}
texts[1] = "Fall of the Hooded Goblin"
texts[2] = "30 Hymns"
texts[3] = "Advanced Rocket Science for Farmers"
texts[4] = "The Ancient Art of Brew"
texts[5] = "White Magic Reference Manual"
texts[6] = "Modern Tailoring"
texts[7] = "Side Effects of Various Potions"
texts[8] = "Forgive and Forget"
texts[9] = "Where is the Queen?"

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
	
	local i
	for i=1,#books do
		if (activated == books[i].id) then
			doDialogue("Eny: \"" .. texts[i] .. "\"...\n")
		end
	end
	for i=1,#books2 do
		if (activated == books2[i].id) then
			doDialogue("Eny: \"" .. texts[i] .. "\"...\n")
		end
	end
end

function collide(id1, id2)
end

