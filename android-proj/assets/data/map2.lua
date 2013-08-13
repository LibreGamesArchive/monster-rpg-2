
--[[


This describes map locations and their
relationship to one another. The "milestone"
field must be met for traversal to that
location to be allowed.
--]]

landing = {
	name="Landing Area",
	x=108,
	y=275,
	milestone=true,
	dest_area="moon_landing",
	dest_x=10,
	dest_y=13,
	dest_dir=DIRECTION_NORTH,
}

shyzu = {
	name="Shyzu",
	x=88,
	y=255,
	milestone=true,
	dest_area="shyzu",
	dest_x=0,
	dest_y=7,
	dest_dir=DIRECTION_EAST,
}

valley = {
	name="Moon Valley",
	x=112,
	y=237,
	milestone=getMilestone(MS_TIPPER_JOINED),
	dest_area="moon_valley",
	dest_x=14,
	dest_y=199,
	dest_dir=DIRECTION_NORTH,
}


muttrace = {
	name="Muttrace",
	x=159,
	y=122,
	milestone=getMilestone(MS_PAST_MOON_VALLEY),
	dest_area="Muttrace",
	dest_x=20,
	dest_y=29,
	dest_dir=DIRECTION_NORTH,
}

darkside = {
	name="Dark Side",
	x=187,
	y=47,
	milestone=getMilestone(MS_LANDER_LAUNCHED),
	dest_area="darkside",
	dest_x=71,
	dest_y=49,
	dest_dir=DIRECTION_NORTH,
}


--[[
These specify what location in is each
direction from the location of the
table given.
]]--

area_table_names = {
	"landing",
	"shyzu",
	"valley",
	"muttrace",
	"darkside",
}

num_areas = #area_table_names

function get_area_table_name(i)
	return area_table_names[i]
end


landing.left = nil
landing.right = nil
landing.up = "shyzu"
landing.down = nil

shyzu.left = nil
shyzu.right = nil
shyzu.up = "valley"
shyzu.down = "landing"

valley.left = nil
valley.right = nil
valley.up = "muttrace"
valley.down = "shyzu"

muttrace.left = nil
muttrace.right = nil
muttrace.up = "darkside"
muttrace.down = "valley"

darkside.left = nil
darkside.right = nil
darkside.up = nil
darkside.down = "muttrace"

function post_cfg()
end

