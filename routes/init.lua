Add_Route('/hello', "GET", "<html><h1>Hello</h1></html>")

Add_Route("/test", "GET", function()
	local file = io.open('./server/test.html', "r")
	if not file then
		return "<html><h1>404</h1></html>"
	end
	local lines = file:read("*a")
	file:close()
	return lines
end)
