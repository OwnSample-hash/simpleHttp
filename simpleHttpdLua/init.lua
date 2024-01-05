if scan_ == nil or log_log == nil then
  print('LUA FAILED NO GLOBAL FUNCS ARE PUSHED')
  print("scan:" .. tostring(scan))
  print("log_log:" .. tostring(log_log))
end


---Route registration wrapper
---@param path string
---@param meth string
---@param fn function|string
---@return nil
function Add_Route(path, meth, fn)
  if path == "/" then
    log(LOG_WARN, "Adding route to path '/' will not be used TODO:fix")
    return
  end
  Funcs[path] = fn
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
  if caller == nil then
    log_log(LOG_ERROR, "No caller")
    print("NO CALLER")
  end
  log_log(lvl, caller.short_src, caller.currentline, string.format(fmt, ...))
end

SERVER_ROOT = "./server/"

if not pcall(function()
      require 'routes'
    end) then
  log(LOG_ERROR, "faild to do routes")
end


---Scans a dir
---@param path string
---@return table(string)
function Scan(path)
  return scan_(path)
end

log(LOG_INFO, "Adding routes automaticlly")
local routes_dir = "routes/"
local routes_scan = Scan(routes_dir)

if routes_scan then
  if type(routes_scan) == 'table' then
    for k, v in pairs(routes_scan) do
      log(LOG_INFO, k .. ":" .. tostring(v))
      if k == (routes_dir .. "init.lua") then
        require(routes_dir)
        log(LOG_TRACE, "%sinit.lua added", routes_dir)
      end
    end
  else
    log(LOG_WARN, routes_scan)
  end
else
  log(LOG_WARN, "routes_scan is empty")
end
