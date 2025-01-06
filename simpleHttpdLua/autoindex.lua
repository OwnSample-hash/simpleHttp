---Creates Auto Index from path
---@param fpath string
---@param vpath string
function CreateAutoIndex(fpath, vpath)
  Info("Creating %s for %s", vpath, fpath)
  Add_Route(vpath, "GET", "auto", function()
    -- no recursive scan
    -- accept dir
    -- auto add dir to vpath function or mark the main vpath as AutoIndex
    -- do as while that dir is the cd; nftw flag

    local contents = Scan(fpath, FTW.PHYS)
    for k, v in pairs(contents) do
      print(k .. ":" .. tostring(v.path))
    end
    return Template.DoFile("./simpleHttpdLua/autoindex.html", { files = contents })
  end)
end

-- Vim: set expandtab tabstop=2 shiftwidth=2:
