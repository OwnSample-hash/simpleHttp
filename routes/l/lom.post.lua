return function(payload)
	Trace("payload %s", payload)
	return { code = 200, body = "Hello World" }
end
-- Vim: set expandtab tabstop=2 shiftwidth=2:
