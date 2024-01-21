PORT = 8080
IP = "::1" --"127.0.0.1"
DOMAIN = "AF_INET6"
LISTEN = 5
PROTO = "HTTP"

SERVER_ROOT = "./server2"
ROUTES_ROOT = "./routes"

create_socket(IP, PORT, DOMAIN, LISTEN, PROTO)

set_server_root(SERVER_ROOT)
set_routes_root(ROUTES_ROOT)
