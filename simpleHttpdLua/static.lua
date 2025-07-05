---Creates Static Server from path
---@param fpath string
---@param vpath string|nil If nil, then vpath is the same as fpath
function CreateStaticServer(fpath, vpath)
	Info("Creating Static Server for %s", fpath)
	if fpath == nil then
		Error("Path is nil, cannot create static server")
		return
	end
	if vpath == nil then
		if fpath == "/" then
			Warn("Creating static server for root path, this may cause issues")
		end
		vpath = fpath
	end
	Add_Route(vpath, "GET", "auto", function()
		-- if __PATH == "/" then
		-- 	__PATH = "index.html"
		-- end
		Info("Serving static file %s", vpath)
		Trace("Serving static file %s", fpath .. __PATH)
		return "$!STATIC"
	end)
end
-- Vim: set expandtab tabstop=2 shiftwidth=2:
