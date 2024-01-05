Add_Route('/hello', "GET", "<html><h1>Hello</h1></html>")

Add_Route("/test", "GET", function()
	local file = io.open(SERVER_ROOT .. '/test.html', "r")
	if not file then
		return "<html><h1>404</h1></html>"
	end
	local lines = file:read("*a")
	file:close()
	return lines
end)

Add_Route("/lom", "GET", function()
	return "<html><h1>" .. tostring(math.random()) .. "</h1></html>"
end)

Add_Route("/scan", "GET", function()

end)

log(LOG_INFO, "Added routes")
