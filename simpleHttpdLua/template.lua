Template = Template or {}

Template.html = Template.html or {}

Template.html.current_ret = ""

function Template.html.li(str)
  Template.html.current_ret = Template.html.current_ret .. "<li>" .. tostring(str) .. "</li>\n"
end

function Template.html.title(str)
  Template.html.current_ret = "<title>" .. tostring(str) .. "</title>"
end

---Processes a function chunk
---@param code string
---@param args table
local function ProcessFunction(code, args)
  local env = Template.joinTables(Template.html, args,
    {
      print = print,
      pairs = pairs,
      next = next,
      ipairs = ipairs,
      tostring = tostring
    }
  )
  local fn = load(code, nil, "t", env)
  if not fn then
    print(fn)
    log(LOG_TRACE, code)
    print(code)
    Die("Code is nil")
    return
  end
  fn()
end

---Template a lua html file, coro function
---@param path string
---@param args table
function Template.DoFileCoro(path, args)
  log(LOG_INFO, "Templating path(%s) with args(%s)", path, args)
  local file = io.open(path)
  if not file then
    log(LOG_ERROR, "Can't open file %s", path)
    return
  end
  local function_code = "%s"
  local dump_code = false
  for line in file:lines() do
    if line:find("^%s*</lua_script>") then
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
    if line:find("^%s*<lua_script>") then dump_code = true end
    if not dump_code then
      coroutine.yield(line)
    end
    ::continue::
  end
  function_code = function_code:format("")
  log(LOG_TRACE, function_code)
  file:close()
end

---Join two tables together
---@param t1 table
---@param t2 table
---@param t3 table
---@return table
function Template.joinTables(t1, t2, t3)
  local result = {}
  result.html = {}
  for k, v in pairs(t1) do
    result.html[k] = v
  end
  for k, v in pairs(t2) do
    result[k] = v
  end
  for k, v in pairs(t3) do
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
    ret = ret .. processedLine
  end
  if not success then
    log(LOG_ERROR, "Coroutine error: %s", processedLine)
  end
  return ret
end
