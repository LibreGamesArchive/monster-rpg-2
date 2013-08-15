music = "monastery.ogg"

function start()
	add_downward_light(104, 24, 48, 100, 128, 255, 255, 150, 80)
	add_downward_light(104, 64, 48, 80, 68, 255, 255, 150, 80)

	out = Portal:new{x=6, y=14}

	monk0 = Object:new{x=2, y=6, anim_set="monk0"}
	monk1 = Object:new{x=4, y=6, anim_set="monk0"}
	monk2 = Object:new{x=6, y=6, anim_set="monk0"}
	monk3 = Object:new{x=8, y=6, anim_set="monk1"}
	monk4 = Object:new{x=10, y=6, anim_set="monk0"}
	monk5 = Object:new{x=2, y=8, anim_set="monk1"}
	monk6 = Object:new{x=4, y=8, anim_set="monk0"}
	monk7 = Object:new{x=6, y=8, anim_set="monk1"}
	monk8 = Object:new{x=8, y=8, anim_set="monk0"}
	monk9 = Object:new{x=10, y=8, anim_set="monk0"}
end

function stop()
end

function update(step)
	if (out:update()) then
		change_areas("flowey", 16, 12, DIRECTION_SOUTH)
	end
end

function activate(activator, activated)
	if (activated == monk0.id) then
		doDialogue("Speed is important to start, but everyone gets a turn...\n")
	elseif (activated == monk1.id) then
		doDialogue("There are 3 elements of magic...\nFire, Ice, and Electricity...\nFire burns things that are flammable. Ice freezes hot things. Electricity blasts water elementals... usually.\n")
	elseif (activated == monk2.id) then
		doDialogue("Some of the nicest treasures are the easiest to overlook...\n")
	elseif (activated == monk3.id) then
		doDialogue("If someone wants something you have, do not hesitate to trade with him...\n")
	elseif (activated == monk4.id) then
		doDialogue("Talk to everyone and you will gain much knowledge...\n")
	elseif (activated == monk5.id) then
		doDialogue("It's not how much you make, but how you spend it...\n")
	elseif (activated == monk6.id) then
		doDialogue("It is important to know your enemy's weaknesses...\n")
	elseif (activated == monk7.id) then
		doDialogue("Use formations to your advantage...\nOh, you don't know about formations?\nIf you are closer to the enemy, you do more damage...\n...but if you are farther away, you take less of a hit.\n")
	elseif (activated == monk8.id) then
		doDialogue("Take a rest and record your progress whenever you can...\n")
	elseif (activated == monk9.id) then
		doDialogue("Train wisely and balance your skills... a lopsided warrior is easy to knock down...\n")
	end
end

function collide(id1, id2)
end

