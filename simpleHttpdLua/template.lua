Template = Template or {}

Template.html = Template.html or {}

Template.html.current_ret = ""
Template.html.prev_ret = ""

---@class attrs
---@field key string @Key of the attribute
---@field value string @Value of the attribute
local attrs = {}

---@class node
---@field name string @Name of the node
---@field children table @Array of children nodes
---@field attrs attrs[] @Array of attributes of the node
------@field data string @Data of the node
local node = {}

---@class nodes
---@field tags node|nil @Array of tags to be used in the template in one <lua> section
local nodes = {}

---@type nodes
Template.html.nodes = {}

function Template.html.html_tag_old(tbl, key)
	for k, v in pairs(tbl) do
		if k == key then
			return v
		end
	end
	return function(data, attributes)
		Debug("Creating tag %s", key)
		local line = "<" .. key
		if attributes then
			for k, v in pairs(attributes) do
				line = line .. " " .. k .. '="' .. v .. '"'
			end
		end
		line = line .. ">" .. tostring(data) .. "</" .. key .. ">\n"
		if Template.html.prev_ret ~= "" then
			Template.html.current_ret = Template.html.prev_ret
		end
		Template.html.prev_ret = Template.html.current_ret
		Template.html.current_ret = Template.html.current_ret .. line
		return {
			["str"] = line,
			__tostring = function(self)
				return self.str
			end,
		}
	end
end

function Template.html.html_tag(tbl, key)
	for k, v in pairs(tbl) do
		if k == key then
			return v
		end
	end

	return setmetatable({}, {
		__call = function(_, data, attributes)
			Debug("Creating tag %s", key)

			-- Start building the tag
			local line = "<" .. key
			if attributes then
				for k, v in pairs(attributes) do
					line = line .. " " .. k .. '="' .. v .. '"'
				end
			end
			line = line .. ">" .. tostring(data) .. "</" .. key .. ">\n"

			-- Update Template.html tracking
			-- if Template.html.prev_ret ~= "" then
			--   Template.html.current_ret = Template.html.prev_ret
			-- end
			-- Template.html.prev_ret = Template.html.current_ret
			Template.html.current_ret = Template.html.current_ret .. line

			-- Return a callable object for chaining
			return setmetatable({
				str = line,
				__tostring = function(self)
					return self.str
				end,
			}, getmetatable(_))
		end,

		-- __index for nested chaining (e.g., a(h1(...), {href="/"}))
		__index = function(_, subkey)
			return Template.html.html_tag(tbl, subkey)
		end,
	})
end

---Processes a function chunk
---@param code string
---@param args table
local function ProcessFunction(code, args)
	local env = Template.joinTables(args, {
		print = print,
		pairs = pairs,
		next = next,
		ipairs = ipairs,
		tostring = tostring,
		tonumber = tonumber,
		raw = function(str)
			Template.html.current_ret = Template.html.current_ret .. tostring(str)
		end,
		endloop = function()
			Error("perv_ret = %s", Template.html.prev_ret)
			Template.html.prev_ret = ""
		end,
		json = Dump_table,
	})
	setmetatable(env, { __index = Template.html.html_tag })
	Trace("Processing function with env")
	for k, v in pairs(env) do
		Trace("Key: %s, Value: %s", k, v)
	end
	local fn = load(code, nil, "t", env)
	if not fn then
		print(fn)
		Trace(code)
		Die("Code is nil")
		return
	end
	fn()
end

---Template a lua html file, coro function
---@param path string
---@param args table
function Template.DoFileCoro(path, args)
	Info("Templating path(%s) with args(%s)", path, args)
	local file = io.open(path)
	if not file then
		Error("Can't open file %s", path)
		return
	end
	local function_code = "%s"
	local dump_code = false
	for line in file:lines() do
		if line:find("^%s*</lua>") then
			dump_code = false
			function_code = function_code:format("\n")
			ProcessFunction(function_code, args)
			function_code = "%s"
			coroutine.yield(Template.html.current_ret)
			Template.html.current_ret = ""
			goto continue
		end
		if dump_code then
			function_code = function_code:format(line) .. "%s"
		end
		if line:find("^%s*<lua>") then
			dump_code = true
		end
		if not dump_code then
			coroutine.yield(line)
		end
		::continue::
	end
	function_code = function_code:format("")
	Trace(function_code)
	file:close()
end

---Join two tables together
---@param t1 table
---@param t2 table
---@return table
function Template.joinTables(t1, t2)
	local result = {}
	for k, v in pairs(t1) do
		result[k] = v
	end
	for k, v in pairs(t2) do
		result[k] = v
	end
	return result
end

---Template a lua html file
---@param path string
---@param args table
function Template.DoFile(path, args)
	local co = coroutine.create(Template.DoFileCoro)
	local success, processedLine = coroutine.resume(co, path, args)
	local ret = processedLine
	while success and coroutine.status(co) ~= "dead" do
		success, processedLine = coroutine.resume(co)
		ret = ret .. (processedLine or "")
	end
	if not success then
		Error("Coroutine error: %s", processedLine)
	end
	return ret
end

-- Vim: set expandtab tabstop=2 shiftwidth=2:
