CC = clang
CFLAGS = -ggdb -DGIT_COMMIT=\"$(shell git rev-parse --short HEAD)\" -DLOG_USE_COLOR=1 -masm=intel -pg
LDFLAGS = -lmagic -llua -pg -lm -ldl

BIN = simpleHttpd

_MAKE_DIR = make.dir
SRC_DIR = src
BUILD_DIR = ${_MAKE_DIR}/build

CSRCS = $(shell find ${SRC_DIR} -type f -name "*.c")

OBJS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(CSRCS))

all: backup ${BIN}
	@echo Done bulding

backup:
	@if [[ -f ${BIN} ]]; then \
		mv ${BIN} ${BIN}.prev; \
	fi

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
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN): $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $(BIN)

clean:
	rm ${BUILD_DIR} ${LUA_VER} ${BIN} -rf
