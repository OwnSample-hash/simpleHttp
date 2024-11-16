SpecRet = SpecRet or {}

function SpecRet._404()
	Log(LOG_WARN, "404 req for client %d", __CFD)
	local header = ""
	Log(LOG_TRACE, "Wrote %d bytes to socket", Write(__CFD, header, header:len()))
	return "$!HANDELD"
end

---301 returns
---@param location string
function SpecRet._301(location)
	Log(LOG_WARN, "301 req for client %d, location: %s", __CFD, location)
	local redir = ("HTTP/1.1 301 Moved Permanently\r\nLocation: %s\r\n"):format(location)
	Log(LOG_TRACE, "%s", redir)
	Log(LOG_TRACE, "Wrote %d bytes", Write(__CFD, redir, redir:len()))
	return "$!HANDELD"
end

--[[
"HTTP/1.1 404 Not Found\r\n"
"Connection: close\r\n"
"Content-Length: 65\r\n"
"Content-Type: text/html\r\n"
"\r\n"
"<html>\r\n"
"<head>\r\n"
"</head>\r\n"
"<body>\r\n"
"<h1>404</h1>\r\n"
"</body>\r\n"
"</html>\r\n";
--]]
