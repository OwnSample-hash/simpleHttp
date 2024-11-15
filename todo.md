### [src/threadJob.c](src/threadJob.c)
- Make connection keep alive

### [simpleHttpdLua/autoindex.lua](simpleHttpdLua/autoindex.lua)
- no recursive scan
- accept dir
- auto add dir to vpath function or mark the main vpath as AutoIndex
- do as while that dir is the cd; nftw flag
- this thing gonna need a [scan](src/lua/scan.c) rewrite

### [src/parser.c](src/parser.c)
- shit
- no check for HTTP/1.1 in the header
- save user req
- better [Linked List](src/linkList.c)
