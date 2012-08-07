music = "underground.ogg"

function start()
	chest1 = Chest:new{x=36, y=18, milestone=MS_UNDERGROUND_2_CHEST_1, index=ITEM_HOLY_WATER}
	portal1 = Portal:new{x=3, y=8}
	fountain = Object:new{x=16, y=3}
	portal2 = Portal:new{x=34, y=10}
	chest2 = Chest:new{x=33, y=10, anim_set="chest", milestone=MS_UNDERGROUND_2_CHEST_2, index=ITEM_ELIXIR}
	chest3 = Chest:new{x=35, y=10, anim_set="chest", milestone=MS_UNDERGROUND_2_CHEST_3, index=ITEM_IRON_BLADE}
end

function stop()
end


function play_boss_sample()
	loadPlayDestroy("boss.ogg")
end

function defeated_monster()
	setMilestone(MS_DEFEATED_MONSTER, true)
	removeObject(monster.id)
end


function update(step)
	if (not getMilestone(MS_DEFEATED_MONSTER) and not getMilestone(MS_ENCOUNTERED_MONSTER)) then
		px, py = getObjectPosition(0)

		if ((px == 26) and (py == 14)) then
			stopObject(0)
			monster = Object:new{x=27, y=14, anim_set="Monster"}
			scriptifyPlayer();
			eny = Object:new{id=0, x=26, y=14}
			setObjectDirection(0, DIRECTION_EAST)
			eny.scripted_events = {
				{ event_type=EVENT_CUSTOM, callback=play_boss_sample },
				{ event_type=EVENT_REST, delay=2000 },
				{ event_type=EVENT_BATTLE, id="Monster" },
				{ event_type=EVENT_WAIT_FOR_BATTLE },
				{ event_type=EVENT_DESCRIPTIFY },
				{ event_type=EVENT_CUSTOM, callback=defeated_monster },
			}
			setMilestone(MS_ENCOUNTERED_MONSTER, true)
		end
	end

	if (not getMilestone(MS_DEFEATED_MONSTER) and getMilestone(MS_ENCOUNTERED_MONSTER)) then
		eny:update(step)
	end

	portal1:update()
	
	if (portal1.go_time) then
		change_areas("Underground_1", 35, 34, DIRECTION_NORTH);
	end

	portal2:update()

	if (portal2.go_time) then
		change_areas("forest", 13, 32, DIRECTION_EAST);
	end
end

function activate(activator, activated)
	if (activated == chest1.id) then
		chest1:activate()
	elseif (activated == chest2.id) then
		chest2:activate()
	elseif (activated == chest3.id) then
		chest3:activate()
	elseif (activated == fountain.id) then
		addSparklySpiralAroundPlayer()
		reviveAllPlayers()
		loadPlayDestroy("Revive.ogg")
	end
end

function collide(id1, id2)
end

