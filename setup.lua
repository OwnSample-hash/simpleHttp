set_log_level(LOG_FATAL)

create_socket("127.0.0.1", 8000, AF_INET, 5, HTTP)
create_socket("::1", 8000, AF_INET6, 5, HTTP)

set_server_root("server")
set_routes_root("routes")

set_keep_alive({
	keep_alive = false,
	timeout = 5,
	max = 500,
})
