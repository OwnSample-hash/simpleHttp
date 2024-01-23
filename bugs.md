# Bugs

## [src/socket.c](src/socket.c#L107-L121)
- getAddressAndPort doesnt work since ipv6 support
- [ ] Is fixed?

## [src/main.c](src/main.c#L45)
- when initializing the main lua env the config struct pointer corrupts
- > reread the config ig agian
- [ ] Is fixed?
