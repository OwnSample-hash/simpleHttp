CC = clang
CLN = clang
CFLAGS = -ggdb -DGIT_COMMIT=\"$(shell git rev-parse --short HEAD)\"
CLNFLAGS = -lmagic

BIN = simple

SRC_DIR = src/
_MAKE_DIR = make.dir

.PHONY: init
init:
	mkdir -p ${_MAKE_DIR}
	cp ${SRC_DIR} ${_MAKE_DIR} -r
	md5sum `find src/ -type f` > ${_MAKE_DIR}/md5.sum

.PHONY: all
all: prolog init link
	@echo Done bulding

prolog:
	@echo CC=${CC}
	@echo CLN=${CLN}
	@echo CFLAGS=${CFLAGS}
	@echo BIN=${BIN}
	@echo

clean:
	rm build -rf
	rm simple -f

build: clean
	mkdir -p build/src/logger
	@for file in `find src/ -name "*.c"`; do\
		printf "%s -c %-20s %-30s %s\n" ${CC} $$file "-o build/$$file.o" '${CFLAGS}';\
		${CC} -c $$file -o build/$$file.o ${CFLAGS};\
	done
	@for file in `find src/ -name "*.h"`; do\
		printf "cp %-20s %s\n" $$file build/$$file;\
		cp $$file build/$$file;\
	done

link: build
	@printf "${CLN} -o ${BIN} ${CLNFLAGS} \n"
	@for file in `find build/src/ -name "*.o"`; do\
		printf "\t%s\n" $$file;\
	done
	@${CLN} -o ${BIN} ${CLNFLAGS} `find build/src/ -name "*.o"`

