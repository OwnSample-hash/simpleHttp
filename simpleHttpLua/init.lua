Funcs = Funcs or {}

---Route registration wrapper
---@param path string
---@param meth string
---@param fn function|string
---@return nil
function Add_Route(path, meth, fn)
  reg_route(path, meth, fn)
  Funcs[path] = fn
end

Add_Route('/hello', GET, "<html><h1>Hello</h1></html>")

Add_Route("/test", GET, function()
  local file = io.open('./server/test.html', "r")
  if not file then
    return "<html><h1>404</h1></html>"
  end
  local lines = file:read("*a")
  file:close()
  return lines
end)
