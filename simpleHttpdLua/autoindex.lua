---Creates Auto Index from path
---@param fpath string
---@param vpath string
function CreateAutoIndex(fpath, vpath)
  log(LOG_INFO, "Creating %s for %s", vpath, fpath)
  Add_Route(vpath, "GET", "auto", function()
    -- no recursive scan
    -- accept dir
    -- auto add dir to vpath function or mark the main vpath as AutoIndex
    -- do as while that dir is the cd; nftw flag
    --
    local contents = Scan(fpath)
    for k, v in pairs(contents) do
      print(k .. ":" .. tostring(v))
    end
    Template.DoFile("./simpleHttpdLua/autoindex.html", { files = contents })
  end)
end
