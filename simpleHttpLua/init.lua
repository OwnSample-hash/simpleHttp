---Route registration wrapper
---@param path string
---@param meth string
---@param fn function|string
---@return nil
function Add_Route(path, meth, fn)
  reg_route(path, meth, fn)
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
    log_log(1, "No caller")
    print("NO CALLER")
  end
  log_log(lvl, caller.short_src, caller.currentline, fmt, ...)
end

SERVER_ROOT = "./server/"

if pcall(function()
      require 'routes'
    end) then
  log(LOG_INFO, "routes are infact exists")
end
