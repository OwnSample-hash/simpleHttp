---@meta _
---meta file for global vars

---C function for scan a dir
---Use the Scan function for scan a dir
---@param path string
---@param flag number
---@return table(string)
function scan_(path, flag) end

--- C function logging a message
--- Use Log
---@param lvl number Lvl for the message
---@param sourceFile string where the Msg comes from
---@param line number which line it froms
---@param fmtedMessage string The formatted message
function log_log(lvl, sourceFile, line, fmtedMessage) end

--- C function for writing direclty into a fd
---@param fd number
---@param data string
---@param data_len number
---@return number
function write_(fd, data, data_len) end

---@type (number)
--- The File descriptor for the connected socket only avalibe in a request ctx
__CFD = 0

---@type(string)
---Server root varaible
SERVER_ROOT = ""

---@type(string)
---Server routes variable
routes_dir = ""
