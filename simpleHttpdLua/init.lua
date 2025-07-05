_Funcs = _Funcs or {}
if scan_ == nil or log_log == nil or _Funcs == nil or write_ == nil then
	print("LUA FAILED NO GLOBAL FUNCS ARE PUSHED")
	print("scan:" .. tostring(scan_))
	print("log_log:" .. tostring(log_log))
	print("_Funcs:" .. tostring(_Funcs))
	print("log_log:" .. tostring(write_))
	os.exit(1)
end

---Writes data to a file descriptor and returns the number of bytes written
---@param fd number
---@param data string
---@param data_len number
---@return number
function Write(fd, data, data_len)
	return write_(fd, data, data_len)
end

---Route registration wrapper
---@param path string
---@param meth string
---@param cntp string Either auto or a mime type
---@param handler function
---@return nil
function Add_Route(path, meth, cntp, handler)
	if path == "/" then
		Warn("Adding route to path '/' will not be used TODO: fix")
		-- return
	end
	-- TODO: make fn a table
	local index = 0
	for i, v in ipairs(_Funcs) do
		if v["path"] == path then
			index = i
			break
		end
	end
	if index == 0 then
		index = #_Funcs + 1
	end
	local fn = _Funcs[index] or nil
	if fn == nil then
		fn = {
			path = path,
			meth = {
				meth,
			},
			cntp = cntp,
			handler = handler,
		}
		_Funcs[index] = fn
	else
		table.insert(fn.meth, meth)
	end
end

LOG_TRACE = 0
LOG_DEBUG = 1
LOG_INFO = 2
LOG_WARN = 3
LOG_ERROR = 4
LOG_FATAL = 5

---Wraps the log functions from C
---@param lvl number
---@param fmt string
---@param ... any
---@return nil
function Log(lvl, fmt, ...)
	local caller = debug.getinfo(3)
	if not caller then
		print("NO CALLER")
		print(debug.traceback())
		log_log(lvl, "NO CALLER", -1, string.format(fmt, ...))
		return
	end
	log_log(lvl, caller.short_src, caller.currentline, fmt:format(...))
end

--- Log functions
---@param fmt string
---@vararg any
function Trace(fmt, ...)
	Log(LOG_TRACE, fmt, ...)
end

--- Log functions
---@param fmt string
---@vararg any
function Debug(fmt, ...)
	Log(LOG_DEBUG, fmt, ...)
end

--- Log functions
---@param fmt string
---@vararg any
function Info(fmt, ...)
	Log(LOG_INFO, fmt, ...)
end

--- Log functions
---@param fmt string
---@vararg any
function Warn(fmt, ...)
	Log(LOG_WARN, fmt, ...)
end

--- Log functions
---@param fmt string
---@vararg any
function Error(fmt, ...)
	Log(LOG_ERROR, fmt, ...)
end

--- Log functions
---@param fmt string
---@vararg any
function Fatal(fmt, ...)
	Log(LOG_FATAL, fmt, ...)
end

--- Fatal that will exit the server
---@param reason string
function Die(reason)
	local caller = debug.getinfo(2)
	if not caller then
		print("NO CALLER")
	end
	if reason then
		reason = "With the following reason: " .. reason
	else
		reason = ""
	end
	log_log(LOG_FATAL, caller.short_src, caller.currentline, "Server died! " .. reason)
	os.exit(1)
end

Info("Running additonal lua files")
dofile("simpleHttpdLua/special_ret.lua")
dofile("simpleHttpdLua/template.lua")
dofile("simpleHttpdLua/autoindex.lua")
dofile("simpleHttpdLua/static.lua")

MAX_TBL_DEPTH = 5
local tmplt1 = "{%s}"
local tmplt2 = '"%s":"%s"'
local tmplt3 = '"%s": %s'
--- Dumps table into a json string
---@param base_tbl table
---@param depth number | nil
---@return string
function Dump_table(base_tbl, depth)
	depth = depth or 0
	if depth == MAX_TBL_DEPTH then
		return "MAX_TBL_DEPTH REACEHED"
	end
	Warn("dumping table depth: %d", depth)
	local ret_str = ""
	for k, v in pairs(base_tbl) do
		if type(v) == "table" then
			ret_str = ret_str .. tmplt3:format(k, Dump_table(v, depth + 1)) .. ","
		else
			ret_str = ret_str .. tmplt2:format(k, tostring(v)) .. ","
		end
	end
	ret_str = ret_str:sub(1, -2)
	return tmplt1:format(ret_str)
end

FTW = {
	PHYS = 1,
	CD = 4,
}

---Scans a dir
---@param path string
---@param flag number
---@return table(string)
function Scan(path, flag)
	return scan_(path, flag)
end

Log(LOG_INFO, "Adding routes automaticlly")
local routes_scan_raw = Scan(ROUTES_ROOT, FTW.PHYS)
Trace("Returned from scan")
local routes_dir = ROUTES_ROOT or "routes/" -- TODO: use what comes from the config if i made it
Trace("routes_dir %s", ROUTES_ROOT)
local routes_scan = {}

local function table_printer(x)
	for k, v in pairs(x) do
		if type(v) == "table" then
			table_printer(v)
		else
			Log(LOG_DEBUG, "%s: %s", k, tostring(v))
		end
	end
end

Debug(Dump_table(routes_scan_raw))
for _, v in pairs(routes_scan_raw) do
	table.insert(routes_scan, v["path"])
end

if routes_scan then
	if type(routes_scan) == "table" then
		for k, v in pairs(routes_scan) do
			Trace(k .. ":" .. tostring(v))
			if v == (routes_dir .. "init.lua") then
				require(routes_dir)
				Trace("%sinit.lua added", routes_dir)
			else
				Trace("%s is treated as dofile add", v)
				local vpath = v:gsub(routes_dir, "/")
				vpath = vpath:gsub("%.lua", "")
				local tmp = {}
				for split in vpath:gmatch("%a+") do
					table.insert(tmp, split)
				end
				local meth = tmp[#tmp]
				local path = "/"
				Trace("meth: %s", meth)
				for _, v2 in pairs(tmp) do
					if v2 == meth then
						path = path:sub(1, -2)
						break
					end
					path = path .. v2 .. "/"
				end
				Trace("vpath:%s, path:%s, meth:%s", vpath, path, meth)
				local handler = dofile(v)
				Trace("handler %s", handler)
				if handler == nil then
					Warn("Not adding nil path: %s, vpath: %s, meth: %s", path, vpath, meth)
					Error("handler: %s", handler)
				else
					Add_Route(path, meth:upper(), "auto", handler)
					Info("Added path: %s, meth: %s", path, meth)
				end
			end
		end
	else
		Warn(routes_scan)
	end
else
	Warn("routes_scan is empty")
end

CreateAutoIndex("autoindex", "/autoindex*")
CreateStaticServer(SERVER_ROOT, "/*")
-- Vim: set expandtab tabstop=2 shiftwidth=2:
