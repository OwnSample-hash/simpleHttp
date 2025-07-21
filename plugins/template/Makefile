BIN = template

CC=clang
LD=clang
AR=ar
CFLAGS=-ggdb -DBIN_NAME=\"${BIN}\" -DGIT_COMMIT=\"$(shell git rev-parse --short HEAD)\" \
       -DLOG_USE_COLOR=1 -Wextra -Wall 
LDFLAGS=-lmagic -llua -lm -ldl -shared
ARFLAGS=crs

CC_U = ${shell echo ${CC}-CC | sed 's/.*/\U&/'}
LD_U = ${shell echo ${LD}-LD | sed 's/.*/\U&/'}
AR_U = ${shell echo ${AR} | sed 's/.*/\U&/'}

_MAKE_DIR = make.dir
SRC_DIR = src
BUILD_DIR = ${_MAKE_DIR}/build

CSRCS = $(shell find ${SRC_DIR} -type f -name "*.c")

OBJS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(CSRCS))

all: ${BIN}
	@printf "  Done bulding\n"

restore:
	mv ${BIN}.prev ${BIN}

prolog:
	@echo CC=${CC}
	@echo CFLAGS=${CFLAGS}
	@echo LDFLAGS=${LDFLAGS}
	@echo BIN=${BIN}
	@echo BUILD_DIR=${BUILD_DIR}
	@echo FULL_BD=$(patsubst $(SRC_DIR)%, $(BUILD_DIR)%, $(shell find $(SRC_DIR) -type d))
	@echo OBJS=${OBJS}
	@echo PLUGINS_INSTALL_PREFIX=${PLUGINS_INSTALL_PREFIX}
	@echo

$(BUILD_DIR):
	@printf "  %-8s %s%s\n" "MKDIR" "$(PATH_PREFIX)" "$(patsubst $(SRC_DIR)%, $(BUILD_DIR)%, $(shell find $(SRC_DIR) -type d))"
	@mkdir -p $(patsubst $(SRC_DIR)%, $(BUILD_DIR)%, $(shell find $(SRC_DIR) -type d))

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	@printf "  %s  %s\n" ${CC_U} ${shell echo $< | rev | tr '/' '\n' | rev | head -1}
	@$(CC) $(CFLAGS) -c $< -o $@

$(BIN): $(OBJS)
	@printf "  %s  %s\n" $(LD_U) ${BIN}
	@$(LD) $(OBJS) $(LDFLAGS) -o lib$(BIN).so

clean:
	@printf "  %-9s %s\n" "RM" "${LUA_VER} ${LUA_VER}.tar.gz ${BIN}"
	@rm -rf ${BUILD_DIR} ${LUA_VER} ${LUA_VER}.tar.gz lib${BIN}.so

.PHONY: install
install: all
	@printf "  %-9s %s\n" "INSTALL" "${BIN}"
	@cp lib${BIN}.so ${PLUGINS_INSTALL_PREFIX}/

