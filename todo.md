### [src/threadJob.c](src/threadJob.c)
- Make connection keep alive

### [simpleHttpdLua/autoindex.lua](simpleHttpdLua/autoindex.lua)
- no recursive scan
- accept dir
- auto add dir to vpath function or mark the main vpath as AutoIndex
- do as while that dir is the cd; nftw flag
- this thing gonna need a [scan](src/lua/scan.c) rewrite
