CC = clang
CLN = clang
CFLAGS = -ggdb -DGIT_COMMIT=\"$(shell git rev-parse --short HEAD)\"
CLNFLAGS = -lmagic

BIN = simple

.PHONY: all
all: prolog link
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
		printf "${CC} -c %-20s %-30s %s\n" $$file "-o build/$$file.o" '${CFLAGS}';\
		${CC} -c $$file -o build/$$file.o ${CFLAGS};\
	done
	@for file in `find src/ -name "*.h"`; do\
		printf "cp %-20s %s\n" $$file build/$$file;\
		cp $$file build/$$file;\
	done

objects = $(shell find build/src/ -name "*.o" | sed "s#build/##")

link: build
	@cd build;\
	${CLN} -o ${BIN} ${CLNFLAGS} $(objects) 
	@printf "${CLN} -o ${BIN} ${CLNFLAGS} \n"
	@for file in $(objects); do\
		printf "\t%s\n" $$file;\
	done
	cp build/simple .

