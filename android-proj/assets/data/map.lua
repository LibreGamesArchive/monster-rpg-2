
--[[


This describes map locations and their
relationship to one another. The "milestone"
field must be met for traversal to that
location to be allowed.
--]]

-- FIXME: some of these dest_*'s will need to change based on which
-- direction they're being entered from

forest = {
	name="Ohleo Forest",
	x=55,
	y=208,
	milestone=getMilestone(MS_MET_HORSE),
	dest_area="forest",
	dest_x=51,
	dest_y=76,
	dest_dir=DIRECTION_NORTH,
}


seaside = {
	name="Seaside Town",
	x=135,
	y=216,
	milestone=getMilestone(MS_MET_HORSE),
	--dest_area="seaside",
}

if (getMilestone(MS_BEAT_TODE)) then
	seaside.dest_area = "seaside_repaired"
else
	seaside.dest_area = "seaside"
end

if (((prev == "seaside") and getMilestone(MS_LEFT_SEASIDE_BY_BOAT)) or
		prev == "eastern_dock" or getMilestone(MS_LEFT_EAST_BY_BOAT)) then
	seaside.dest_x=50
	seaside.dest_y=15
	seaside.dest_dir=DIRECTION_SOUTH
else
	seaside.dest_x=16
	seaside.dest_y=33
	seaside.dest_dir=DIRECTION_NORTH
end

eastern_dock = {
	name="Eastern Docks",
	x=320,
	y=340,
	milestone=getMilestone(MS_TALKED_TO_CAPTAIN),
	dest_area="eastern_dock",
}


if (((prev == "eastern_dock") and getMilestone(MS_LEFT_EAST_BY_BOAT)) or
		prev == "seaside" or getMilestone(MS_LEFT_SEASIDE_BY_BOAT)) then
	eastern_dock.dest_x=19
	eastern_dock.dest_y=15
	eastern_dock.dest_dir=DIRECTION_SOUTH
elseif (getMilestone(MS_FORT_TO_DOCK) or getMilestone(MS_DOCK_TO_FORT)) then
	eastern_dock.dest_x=24
	eastern_dock.dest_y=15
	eastern_dock.dest_dir=DIRECTION_SOUTH
else
	eastern_dock.dest_x=49
	eastern_dock.dest_y=16
	eastern_dock.dest_dir=DIRECTION_WEST
end


castle = {
	name="King's Castle",
	x=383,
	y=338,
	milestone=getMilestone(MS_TALKED_TO_CAPTAIN),
	dest_area="castle",
	dest_x=22,
	dest_y=51,
	dest_dir=DIRECTION_NORTH,
}


mountains = {
	name="The Gateway Mountains",
	x=374,
	y=290,
	milestone=getMilestone(MS_GUNNAR_JOINED),
	dest_area="mountains",
	dest_x=10,
	dest_y=82,
	dest_dir=DIRECTION_NORTH,
}


flowey = {
	name="Flowey",
	x=357,
	y=251,
	milestone=getMilestone(MS_UNLOCKED_FLOWEY),
	dest_area="flowey",
	dest_x=7,
	dest_y=32,
	dest_dir=DIRECTION_NORTH,
}


beach = {
	name="Marley Beach",
	x=330,
	y=254,
	milestone=getMilestone(MS_RIOS_TALKED_TO_LIBRARIAN),
	dest_area="beach",
	dest_x=8,
	dest_y=0,
	dest_dir=DIRECTION_SOUTH,
}


farm = {
	name="Old Flyer's Farm",
	x=399,
	y=236,
	milestone=getMilestone(MS_BEACH_BATTLE_DONE),
	dest_area="farm",
	dest_x=0,
	dest_y=18,
	dest_dir=DIRECTION_EAST,
}


fortress = {
	name="Floating Fortress",
	x=256,
	y=160,
	milestone=getMilestone(MS_SUB_SCENE_DONE),
	dest_area="fort_start",
	dest_x=36, dest_y=47,
	dest_dir=DIRECTION_SOUTH,
}


keep = {
	name="Ohleo Keep",
	x=72,
	y=105,
	milestone=getMilestone(MS_BEAT_TIGGY),
	dest_area="Keep_outer",
	dest_x=19, dest_y=62,
	dest_dir=DIRECTION_NORTH,
}

jungle = {
	name="Twisted Jungle",
	x=442,
	y=299,
	milestone=getMilestone(MS_SPOKE_TO_KING_ABOUT_STAFF),
	dest_area="jungle",
	dest_x=17, dest_y=98,
	dest_dir=DIRECTION_NORTH,
}

jungle_village = {
	name="Jungle Village",
	x=446,
	y=259,
	milestone=getMilestone(MS_PASSED_JUNGLE),
	dest_area="jungle_village",
	dest_x=24, dest_y=28,
	dest_dir=DIRECTION_NORTH,
}

volcano = {
	name="Quiet Volcano",
	x=454,
	y=237,
	milestone=getMilestone(MS_UNLOCKED_VOLCANO),
	dest_area="volcano",
	dest_x=36, dest_y=71,
	dest_dir=DIRECTION_NORTH,
}


portal = {
	name="Portal",
	x=17,
	y=197,
	milestone=getMilestone(MS_BEAT_TODE),
	dest_area="portal",
	dest_x=0, dest_y=8,
	dest_dir=DIRECTION_EAST,
}

----------


--[[
These specify what location in is each
direction from the location of the
table given.
]]--


-- FIXME: UPDATEME


forest.left = "portal"
forest.right = "seaside"
forest.up = "keep"
forest.down = nil

if (getMilestone(MS_LEFT_SEASIDE_BY_BOAT) or getMilestone(MS_LEFT_EAST_BY_BOAT)) then
	seaside.left = nil
	seaside.right = "eastern_dock"
	seaside.up = nil
	seaside.down = nil


	eastern_dock.left = "seaside"
	eastern_dock.right = nil
	eastern_dock.up = nil
	eastern_dock.down = nil
else
	seaside.left = "forest"
	seaside.right = nil
	seaside.up = nil
	seaside.down = nil


	eastern_dock.left = nil
	eastern_dock.right = "castle"
	eastern_dock.up = nil
	eastern_dock.down = nil
end


if (getMilestone(MS_FORT_TO_DOCK) or getMilestone(MS_DOCK_TO_FORT)) then
	eastern_dock.left = nil
	eastern_dock.right = nil
	eastern_dock.up = "fortress"
	eastern_dock.down = nil
end


castle.left = "eastern_dock"
castle.right = "jungle"
castle.up = "mountains"
castle.down = nil


mountains.left = nil
mountains.right = nil
mountains.up = "flowey" 
mountains.down = "castle"


flowey.left = "beach"
flowey.right = "farm" 
flowey.up = nil
flowey.down = "mountains"

beach.left = nil
beach.right = "flowey"
beach.up = nil
beach.down = nil

farm.left = "flowey"
farm.right = nil
farm.up = nil
farm.down = nil

fortress.left = nil
fortress.right = nil
fortress.up = nil
fortress.down = "eastern_dock"

keep.left = nil
keep.right = nil
keep.up = nil
keep.down = "forest"

jungle.left = "castle"
jungle.right = nil
jungle.up = "jungle_village"
jungle.down = nil

jungle_village.left = nil
jungle_village.right = nil
jungle_village.up = "volcano"
jungle_village.down = "jungle"

volcano.left = nil
volcano.right = nil
volcano.up = nil
volcano.down = "jungle_village"

portal.left = nil
portal.right = "forest"
portal.up = nil
portal.down = nil


----------

--[[
There is surely a better way to do this, but I don't know it yet
--]]


area_table_names = {
	"forest",
	"seaside",
	"eastern_dock",
	"castle",
	"mountains",
	"flowey",
	"beach",
	"farm",
	"fortress",
	"keep",
	"jungle",
	"jungle_village",
	"volcano",
	"portal",
}

num_areas = #area_table_names

function get_area_table_name(i)
	return area_table_names[i]
end



-- run after the rest of this script is processed... only here is
-- prev_table defined
function post_cfg()
--	if (prev_table.y >= mountains.y) then
--		mountains.dest_x=10
--		mountains.dest_y=82
--		mountains.dest_dir=DIRECTION_NORTH
--	else
--		-- FIXME
--	end
end

