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
ROUTES_ROOT = ""

-- INIT LUA

--- C function for creating a socket
---@param ip string
---@param port number
---@param ipv number
---@param listen number
---@param protocol number
function create_socket(ip, port, ipv, listen, protocol) end

--- C function for setting the server root
---@param root string
function set_server_root(root) end

--- C function for setting the routes root
---@param root string
function set_routes_root(root) end

---@alias KeepAliveCfg table<boolean, number, number>

--- C function for setting the keep alive
---@param cfg KeepAliveCfg
function set_keep_alive(cfg) end

---@type number
AF_INET = 2

---@type number
AF_INET6 = 10

---@type number
---HTTP protocol
HTTP = 1

---@type number
---HTTPS protocol
---TODO: Implement
HTTPS = 2
