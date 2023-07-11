.PHONY:
all: link
	@echo Done bulding

clean:
	rm build -rf
	rm simple -f

build: clean
	mkdir -p build/src/logger
	@for file in `find src/ -name "*.c"`; do\
		printf "clang -c %-20s %s.o -ggdb\n" $$file "-o build/$$file";\
		clang -c $$file -o build/$$file.o -ggdb;\
	done
	@for file in `find src/ -name "*.h"`; do\
		printf "cp %-20s %s\n" $$file build/$$file;\
		cp $$file build/$$file;\
	done

objects = $(shell find build/src/ -name "*.o" | sed "s#build/##")

link: build
	@cd build;\
	clang -o simple $(objects) -Isrc/ -Isrc/logger
	@printf "clang -o simple -Isrc/ -Isrc/logger\n"
	@ for file in $(objects); do\
		printf "\t%s\n" $$file;\
	done
	cp build/simple .




