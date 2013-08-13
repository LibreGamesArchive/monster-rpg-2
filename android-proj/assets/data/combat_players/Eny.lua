function get_actions()
	if (not getMilestone(MS_OPENING_SCENE)) then
		return "Defend", "", "", "", ""
	else
		return "Attack", "Item", "Defend", "Run", ""
	end
end

