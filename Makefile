BIN = simpleHttpd

CC = clang
LD = clang
CFLAGS = -ggdb -DBIN_NAME=\"${BIN}\" -DGIT_COMMIT=\"$(shell git rev-parse --short HEAD)\" -DLOG_USE_COLOR=1 -Wextra -Wall 
LDFLAGS = -lmagic -llua -lm -ldl

CC_U = ${shell echo ${CC}-CC | sed 's/.*/\U&/'}
LD_U = ${shell echo ${LD}-LD | sed 's/.*/\U&/'}

_MAKE_DIR = make.dir
SRC_DIR = src
BUILD_DIR = ${_MAKE_DIR}/build

CSRCS = $(shell find ${SRC_DIR} -type f -name "*.c")

OBJS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(CSRCS))

all: ${BIN}
	@printf "\tDone bulding\n"

restore:
	mv ${BIN}.prev ${BIN}

LUA_VER = lua-5.4.6
install_lua:
	rm ${LUA_VER}.tar.gz -f
	rm ${LUA_VER} -rf
	wget https://www.lua.org/ftp/${LUA_VER}.tar.gz
	tar xf ${LUA_VER}.tar.gz
	$(MAKE) -C ${LUA_VER}/ -j
	${eval CFLAGS += -I${LUA_VER}/src/}
	${eval LDFLAGS += -L${LUA_VER}/src/}

prolog:
	@echo CC=${CC}
	@echo CFLAGS=${CFLAGS}
	@echo LDFLAGS=${LDFLAGS}
	@echo BIN=${BIN}
	@echo BUILD_DIR=${BUILD_DIR}
	@echo FULL_BD=$(patsubst $(SRC_DIR)%, $(BUILD_DIR)%, $(shell find $(SRC_DIR) -type d))
	@echo OBJS=${OBJS}
	@echo


$(BUILD_DIR):
	mkdir -p $(patsubst $(SRC_DIR)%, $(BUILD_DIR)%, $(shell find $(SRC_DIR) -type d))

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	@printf "\t%s  %s\n" ${CC_U} ${shell ./turn_fn.sh $<}
	@$(CC) $(CFLAGS) -c $< -o $@

$(BIN): $(OBJS)
	@printf "\t%s  %s\n" $(LD_U) ${BIN}
	@$(LD) $(OBJS) $(LDFLAGS) -o $(BIN)

clean:
	rm ${BUILD_DIR} ${LUA_VER} ${LUA_VER}.tar.gz ${BIN} -rf
