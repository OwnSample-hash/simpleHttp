_Funcs = _Funcs or {}
if scan_ == nil or log_log == nil or _Funcs == nil or write_ == nil then
  print('LUA FAILED NO GLOBAL FUNCS ARE PUSHED')
  print("scan:" .. tostring(scan))
  print("log_log:" .. tostring(log_log))
  print("_Funcs:" .. tostring(_Funcs))
  print("log_log:" .. tostring(write_))
  return nil
end

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
---@param cntp string Either auto or a short hand code
---@param handler function|string
---@return nil
function Add_Route(path, meth, cntp, handler)
  if path == "/" then
    log(LOG_WARN, "Adding route to path '/' will not be used TODO:fix")
    return
  end
  if type(handler) == "string" then
    _Funcs[path] = handler
  else
    _Funcs[path] = {
      ["fn"] = handler,
      ["meth"] = meth or "GET",
      ["cntp"] = cntp or "auto",
    }
  end
  log(LOG_INFO, "Added path (%s) with meth (%s) with ~fn~ to the registry", path, meth)
end

LOG_TRACE = 0
LOG_DEBUG = 1
LOG_INFO = 2
LOG_WARN = 3
LOG_ERROR = 4
LOG_FATAL = 5

---Exposes the log functions from C
---@param lvl number
---@param fmt string
---@param ... string
---@return nil
function log(lvl, fmt, ...)
  local caller = debug.getinfo(2)
  if not caller then
    print("NO CALLER")
  end
  log_log(lvl, caller.short_src, caller.currentline, string.format(fmt, ...))
end

log(LOG_INFO, "Running additonal lua files")
dofile("simpleHttpdLua/special_ret.lua")
dofile("simpleHttpdLua/template.lua")
dofile("simpleHttpdLua/autoindex.lua")

MAX_TBL_DEPTH = 5
function Dump_table(base_tbl, depth)
  depth = depth or 0
  if depth == MAX_TBL_DEPTH then return "MAX_TBL_DEPTH REACEHED" end
  log(LOG_WARN, "dumping table depth: %d", depth)
  local tmplt1 = "{%s}"
  local tmplt2 = "\"%s\":\"%s\""
  local tmplt3 = "\"%s\": %s"
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
  CD = 4
}

---Scans a dir
---@param path string
---@param flag number
---@return table(string)
function Scan(path, flag)
  return scan_(path, flag)
end

log(LOG_INFO, "Adding routes automaticlly")
local routes_dir = "routes/" -- TODO use what comes from the config if i made it
local routes_scan_raw = Scan(routes_dir, FTW.PHYS)
local routes_scan = {}

local function table_printer(x)
  for k, v in pairs(x) do
    if type(v) == "table" then
      table_printer(v)
    else
      log(LOG_DEBUG, "%s: %s", k, tostring("v"))
    end
  end
end

for k, _ in pairs(routes_scan_raw) do
  routes_scan.insert(k)
end


if routes_scan then
  if type(routes_scan) == 'table' then
    for k, v in pairs(routes_scan) do
      log(LOG_TRACE, k .. ":" .. tostring(v))
      if v == (routes_dir .. "init.lua") then
        require(routes_dir)
        log(LOG_TRACE, "%sinit.lua added", routes_dir)
      else
        log(LOG_TRACE, "%s is treated as dofile add", v)
        local vpath = v:gsub(routes_dir, "/")
        vpath = vpath:gsub("%.lua", "")
        local tmp = {}
        for split in vpath:gmatch("%a+") do
          table.insert(tmp, split)
        end
        local meth = tmp[#tmp]
        local path = "/"
        log(LOG_TRACE, "meth: %s", meth)
        for _, v2 in pairs(tmp) do
          if v2 == meth then
            path = path:sub(1, -2)
            break
          end
          path = path .. v2 .. "/"
        end
        log(LOG_TRACE, "vpath:%s, path:%s, meth:%s", vpath, path, meth)
        local handler = dofile(v)
        log(LOG_TRACE, "handler %s", handler)
        if handler == nil then
          log(LOG_WARN, "Not adding nil path: %s, vpath: %s, meth: %s", path, vpath, meth)
          log(LOG_ERROR, "handler: %s", handler)
        else
          Add_Route(path, meth:upper(), "auto", handler)
          log(LOG_INFO, "Added path: %s, meth: %s", path, meth)
        end
      end
    end
  else
    log(LOG_WARN, routes_scan)
  end
else
  log(LOG_WARN, "routes_scan is empty")
end

CreateAutoIndex("autoindex", "/autoindex")
