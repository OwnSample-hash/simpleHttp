SpecRet = SpecRet or {}

function SpecRet._404()
  log(LOG_INFO, "404 ret")
end

---301 returns
---@param location string
function SpecRet._301(location)
  log(LOG_WARN, "301 req for client %d, location: %s", __CFD, location)
  local redir = ("HTTP/1.1 301 Moved Permanently\r\nLocation: %s\r\n"):format(location)
  log(LOG_TRACE, "%s", redir)
  log(LOG_TRACE, "Wrote %d bytes", Write(__CFD, redir, redir:len()))
  return "$!HANDELD"
end
