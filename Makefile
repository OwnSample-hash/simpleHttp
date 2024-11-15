BIN = simpleHttpd

CC = clang
LD = clang
CFLAGS = -ggdb -DBIN_NAME=\"${BIN}\" -DGIT_COMMIT=\"$(shell git rev-parse --short HEAD)\" -DLOG_USE_COLOR=1 -Wextra -Wall 
LDFLAGS = -lmagic -llua -lm -ldl -ggdb

CC_U = ${shell echo ${CC}-CC | sed 's/.*/\U&/'}
LD_U = ${shell echo ${LD}-LD | sed 's/.*/\U&/'}

_MAKE_DIR = make.dir
SRC_DIR = src
BUILD_DIR = ${_MAKE_DIR}/build

CSRCS = $(shell find ${SRC_DIR} -type f -name "*.c")

OBJS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(CSRCS))

all: ${BIN}
	@printf "  Done bulding\n"

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
	@printf "  %-9s %s" "MKDIR" $(patsubst $(SRC_DIR)%, $(BUILD_DIR)%, $(shell find $(SRC_DIR) -type d))
	@mkdir -p $(patsubst $(SRC_DIR)%, $(BUILD_DIR)%, $(shell find $(SRC_DIR) -type d))

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	@printf "  %s  %s\n" ${CC_U} ${shell echo $< | rev | tr '/' '\n' | rev | head -1}
	@$(CC) $(CFLAGS) -c $< -o $@

$(BIN): $(OBJS)
	@printf "  %s  %s\n" $(LD_U) ${BIN}
	@$(LD) $(OBJS) $(LDFLAGS) -o $(BIN)

clean:
	@printf "  %-9s %s\n" "RM" "${LUA_VER} ${LUA_VER}.tar.gz ${BIN}"
	@rm -rf ${BUILD_DIR} ${LUA_VER} ${LUA_VER}.tar.gz ${BIN}
