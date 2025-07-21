-- set_log_level(LOG_FATAL)

plugin_init("./plugin_install")

create_socket("127.0.0.1", 8080, AF_INET, 5, HTTP)
create_socket("::1", 8080, AF_INET6, 5, HTTP)

set_server_root("docs/html")
set_routes_root("routes")

set_keep_alive({
	keep_alive = false,
	timeout = 5,
	max = 500,
})
-- Vim: set expandtab tabstop=2 shiftwidth=2:
