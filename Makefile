BIN:= simpleHttpd

CC:=clang
LD:=clang
CFLAGS:=-ggdb -DBIN_NAME=\"${BIN}\" -DGIT_COMMIT=\"$(shell git rev-parse --short HEAD)\" \
	-DLOG_USE_COLOR=1 -Wextra -Wall \
	$(shell for dir in $(shell find src -type d); do echo -I$$dir; done) 

LDFLAGS:=-lmagic -llua -lm -ldl

CC_U:= ${shell echo ${CC}-CC | sed 's/.*/\U&/'}
LD_U:= ${shell echo ${LD}-LD | sed 's/.*/\U&/'}

_MAKE_DIR = make.dir
SRC_DIR = src
PL_DIR = plugins
BUILD_DIR = ${_MAKE_DIR}/build
PRE_MAKE_DIRS = ${_MAKE_DIR}/pre
PRE_MAKE_CSRCS = ${_MAKE_DIR}/pre_proc/build

CSRCS := $(shell find ${SRC_DIR} -type f -name "*.c")
PRE_MK := $(patsubst $(SRC_DIR)/%.c.sh, ${PRE_MAKE_DIRS}/%.mk, \
	  $(shell find ${SRC_DIR} -type f -name "*.c.sh"))

PLUGINS = $(shell find ${PL_DIR} -type f -name "Makefile" -exec dirname {} \;)
PLUGINS_INSTALL_PREFIX = $(shell realpath plugin_install)

@chain: ${PRE_MK} plugins ${BIN}
include chains.mk

REMOVAL  := 
ADDITION :=

include ${PRE_MK}

CSRCS := $(filter-out $(REMOVAL), $(CSRCS)) ${ADDITION}

PRE_CSRCS:= $(shell find ${PRE_MAKE_DIRS} -type f -name "*.c.sh")

OBJS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(CSRCS))
OBJS := $(patsubst $(PRE_MAKE_CSRCS)/%.c, $(BUILD_DIR)/%.mk.o, $(OBJS))

all: @chain
	@printf "  Done bulding\n"

restore:
	mv ${BIN}.prev ${BIN}

$(PRE_MK): $(shell find ${SRC_DIR} -type f -name "*.c.sh") | $(PRE_MAKE_DIRS)
	@printf "  %-9s %s\n" "GEN" "$@"
	@mkdir -p $(dir $@)
	@PATH=$(shell realpath tools/):$$PATH bash $< ${PRE_MAKE_DIRS} 


LUA_VER = lua-5.4.7
install_lua:
	@printf "  %-9s %s\n" "RM" "${LUA_VER} ${LUA_VER}.tar.gz"
	@rm ${LUA_VER} ${LUA_VER}.tar.gz -fr
	@printf "  %-9s %s\n" "WGET" "https://www.lua.org/ftp/${LUA_VER}.tar.gz"
	@printf "  %-9s %s\n" "TAR" "${LUA_VER}.tar.gz"
	@curl https://www.lua.org/ftp/${LUA_VER}.tar.gz | tar xvz
	$(MAKE) -C ${LUA_VER}/ -j CFLAGS+="-ggdb"
	${eval CFLAGS += -I${LUA_VER}/src/}
	${eval LDFLAGS += -L${LUA_VER}/src/}

prolog:
	@echo CC=${CC}
	@echo CFLAGS=${CFLAGS}
	@echo LDFLAGS=${LDFLAGS}
	@echo BIN=${BIN}
	@echo BUILD_DIR=${BUILD_DIR}
	@echo FULL_BD=$(patsubst $(SRC_DIR)%, $(BUILD_DIR)%, $(shell find $(SRC_DIR) -type d))
	@echo PRE_CSRCS=${PRE_CSRCS}
	@echo PRE_MK=${PRE_MK}
	@echo CSRCS="${CSRCS}"
	@echo OBJS="${OBJS}"
	@echo
	@echo ADDITION=${ADDITION}
	@echo REMOVAL=${REMOVAL}
	@echo
	@echo PLUGINS=${PLUGINS}
	@echo PLUGINS_INSTALL_PREFIX=${PLUGINS_INSTALL_PREFIX}
	@echo
	@echo "Building ${BIN} with plugin support"
	@echo "Plugins found:"
	@for plugin in ${PLUGINS}; do \
		if [ -f $$plugin/Makefile ]; then \
			$(MAKE) -C $$plugin PATH_PREFIX=$$plugin PLUGINS_INSTALL_PREFIX=${PLUGINS_INSTALL_PREFIX} prolog; \
		else \
			printf "  %-9s %s\n" "SKIP" "$$plugin"; \
		fi; \
	done
	@echo


$(BUILD_DIR):
	@printf "  %-8s %s\n" "MKDIR" "$(patsubst $(SRC_DIR)%, $(BUILD_DIR)%, $(shell find $(SRC_DIR) -type d))"
	@printf "  %-8s %s\n" "MKDIR" "$(patsubst $(SRC_DIR)%, $(PRE_MAKE_DIRS)%, $(shell find $(SRC_DIR) -type d))"
	@mkdir -p $(patsubst $(SRC_DIR)%, $(BUILD_DIR)%, $(shell find $(SRC_DIR) -type d))
	@mkdir -p $(patsubst $(SRC_DIR)%, $(PRE_MAKE_DIRS)%, $(shell find $(SRC_DIR) -type d))

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	@printf "  %s  %s\n" ${CC_U} ${shell echo $< | rev | tr '/' '\n' | rev | head -1}
	@$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.mk.o: $(PRE_MAKE_CSRCS)/%.c | $(BUILD_DIR)
	@printf "  %s  %s\n" ${CC_U} ${shell echo $< | rev | tr '/' '\n' | rev | head -1}
	@$(CC) $(CFLAGS) -c $< -o $@

$(BIN): $(OBJS)
	@printf "  %s  %s\n" $(LD_U) ${BIN}
	@$(LD) $(OBJS) $(LDFLAGS) -o $(BIN)

.PHONY: plugins
plugins:
	@printf "  %-9s %s\n" "MAKE" "plugins"
	@for plugin in ${PLUGINS}; do \
		if [ -f $$plugin/Makefile ]; then \
			$(MAKE) -C $$plugin PATH_PREFIX=$$plugin PLUGINS_INSTALL_PREFIX=${PLUGINS_INSTALL_PREFIX} install; \
		else \
			printf "  %-9s %s\n" "SKIP" "$$plugin"; \
		fi; \
	done

clean:
	@printf "  %-9s %s\n" "RM" "${LUA_VER} ${LUA_VER}.tar.gz ${BIN}"
	@rm -rf ${BUILD_DIR} ${LUA_VER} ${LUA_VER}.tar.gz ${BIN}
	@printf "  %-9s %s\n" "RM" "plugins"
	@for plugin in ${PLUGINS}; do \
		if [ -f $$plugin/Makefile ]; then \
			$(MAKE) -C $$plugin clean; \
		else \
			printf "  %-9s %s\n" "SKIP" "$$plugin"; \
		fi; \
	done

generate_plugin_params:
	@printf "  %-9s %s\n" "GEN" "plugin parameters"
	@./gen_pl_param.py src/plugin/simple_http_plugin.h src/plugin/plugin_params.h
