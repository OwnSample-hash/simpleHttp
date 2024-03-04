# Bugs


## [src/main.c](src/main.c#L45)
- when initializing the main lua env the config struct pointer corrupts
- > reread the config ig agian
- [x] Is fixed?
- - fixed by copy out the read strings to a drv struct

## [src/socket.c](src/socket.c#L107-L121)
- ~~getAddressAndPort doesnt work since ipv6 support~~
- ~~the sockaddr addr struct is corrupt~~
- Now resolvs ipv6 but not ipv4 and also the address family for ipv4 is 0 hmmm
- [x] Is fixed?
- - fixed by using some better shit idk check [src/threa_job.c](src/socket.c)
