if (getMilestone(MS_KILLED_GOLEMS)) then
	music = "Flowey.ogg"
end

function start()
	out = Portal:new{x=7, y=10}
	down = Portal:new{x=7, y=3}

	girl = Object:new{x=7, y=5, anim_set="geisha", person=true, move_type=MOVE_WANDER}

	books = {}
	books[1] = Object:new{x=1, y=3}
	books[2] = Object:new{x=3, y=3}
	books[3] = Object:new{x=5, y=3}
	books[4] = Object:new{x=8, y=3}
	books[5] = Object:new{x=10, y=3}
	books[6] = Object:new{x=12, y=3}
	books[7] = Object:new{x=1, y=6}
	books[8] = Object:new{x=3, y=6}
	books[9] = Object:new{x=10, y=6}
	books[10] = Object:new{x=12, y=6}
	books2 = {}
	books2[1] = Object:new{x=1+1, y=3}
	books2[2] = Object:new{x=3+1, y=3}
	books2[3] = Object:new{x=5+1, y=3}
	books2[4] = Object:new{x=8+1, y=3}
	books2[5] = Object:new{x=10+1, y=3}
	books2[6] = Object:new{x=12+1, y=3}
	books2[7] = Object:new{x=1+1, y=6}
	books2[8] = Object:new{x=3+1, y=6}
	books2[9] = Object:new{x=10+1, y=6}
	books2[10] = Object:new{x=12+1, y=6}
end

function stop()
end

function update(step)
	if (out:update()) then
		change_areas("flowey", 13, 19, DIRECTION_SOUTH)
	elseif (down:update()) then
		change_areas("libb", 7, 3, DIRECTION_SOUTH)
	end

	girl:move(step)
end

texts = {}
texts[1] = "Mathematics"
texts[2] = "Spell Lore"
texts[3] = "Crucial Combat Techniques"
texts[4] = "Mushroom Cultivation"
texts[5] = "Astrological Observation"
texts[6] = "Strong Vine, Good Wine"
texts[7] = "Gender Stereotypes for Dummies"
texts[8] = "Moon People Illustrated"
texts[9] = "Legendary Fairies"
texts[10] = "Maps of the Eastern Kingdom"

function activate(activator, activated)
	if (activated == girl.id) then
		setObjectDirection(girl.id, player_dir(girl))
		if (getMilestone(MS_BEAT_TODE)) then
			doDialogue("Your friend finished his work here...\n")
		elseif (getMilestone(MS_BEACH_BATTLE_DONE)) then
			doDialogue("My father is helping your friend downstairs... He has so much knowledge, oh my!\n")
		else
			doDialogue("One day I will manage this library just as my father has for so many years...\n")
		end
	end

	local i
	for i=1,#books do
		if (activated == books[i].id) then
			doDialogue(_t("Eny") .. ": \"" .. _t(texts[i]) .. "\"...\n")
		end
	end
	for i=1,#books2 do
		if (activated == books2[i].id) then
			doDialogue(_t("Eny") .. ": \"" .. _t(texts[i]) .. "\"...\n")
		end
	end
end

function collide(id1, id2)
end

