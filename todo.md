### [simpleHttpdLua/autoindex.lua](simpleHttpdLua/autoindex.lua)
- no recursive scan
- accept dir
- auto add dir to vpath function or mark the main vpath as AutoIndex
- do as while that dir is the cd; nftw flag
- this thing gonna need a [scan](src/lua/scan.c) rewrite

### Argument Parser

- [ ] add a `--help` option
- [ ] add a `--version` option
- [ ] add a `--config` option to specify a config file
- [ ] add a `--daemon` option to run the server in the background
- [ ] add a `--port` option to specify the port number
- [ ] add a `--host` option to specify the host address
- [ ] add a `--log` option to specify the log file
- [ ] add a `--pidfile` option to specify the PID file
- [ ] add a `--ssl` option to enable SSL support
- [ ] add a `--ssl-cert` option to specify the SSL certificate file
- [ ] add a `--ssl-key` option to specify the SSL key file  
- [ ] add a `--ssl-ca` option to specify the SSL CA file
- [ ] add a `--ssl-ciphers` option to specify the SSL ciphers
- [ ] add a `--ssl-protocols` option to specify the SSL protocols
- [ ] add a `--ssl-verify` option to enable SSL certificate verification
- [ ] add a `--ssl-crl` option to specify the SSL CRL file
- [ ] add a `--ssl-dhparam` option to specify the SSL DH parameters file
- [ ] add a `--ssl-session-timeout` option to specify the SSL session timeout


### Plugin System

### Fix mime detection
- [x] use a database of mime types
- [x] fallback to libmagic if no mime type is found
