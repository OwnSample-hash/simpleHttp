# Bugs


## [src/main.c](src/main.c#L45)
- when initializing the main lua env the config struct pointer corrupts
- > reread the config ig agian
- [x] Is fixed?
- - fixed by copy out the read strings to a drv struct

## [src/socket.c](src/socket.c#L107-L121)
- getAddressAndPort doesnt work since ipv6 support
- the sockaddr addr struct is corrupt 
- [ ] Is fixed?
