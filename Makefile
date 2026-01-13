.PHONY: clean run

ifeq (${DEBUG}, 1)
  CFLAGS += -DDEBUG -O0 -ggdb -fno-inline	
  WRAP   := valgrind --track-origins=yes --leak-check=full --show-leak-kinds=all 
else
  CFLAGS += -O3
endif

LIBFLAGS := $$(pkgconf --cflags ncurses readline sqlite3)
CFLAGS   += -std=c2x -I./source/ -I./object/ -I./ ${LIBFLAGS}
CFLAGS   += -Wall -Wextra -Wpedantic 
LDLIBS   += $$(pkgconf --libs ncurses readline sqlite3) 

OBJECT.d:=object/
SOURCE.d:=source/
SOURCE:=argument_yy.tab.c bash_history.yy.c main.c cli.c tui.c storage.c
OBJECT:=$(addprefix ${OBJECT.d},$(addsuffix .o,$(basename ${SOURCE})))
SOURCE:=$(addprefix ${SOURCE.d},${SOURCE})

OUTPUT:=histui

${OUTPUT}: ${OBJECT}
	${LINK.c} -o ${OUTPUT} ${OBJECT} ${LDLIBS}

object/%.yy.c: source/%.l
	flex --prefix=$*_ --header-file=$(basename $@).h -o $@ $<

object/%.tab.c: source/%.y
	bison --name-prefix=$*_ --header=$(basename $@).h -o $@ $<

object/%.o: object/%.c
	${COMPILE.c} $< -o $@

object/%.o: source/%.c
	${COMPILE.c} $< -o $@

clean:
	-${RM} ${OBJECT.d}/*
	-${RM} ./${OUTPUT}

test:
	${WRAP} ./${OUTPUT}

docs:
	kramdown-man documentation/histui.1.md -o ${OBJECT.d}/histui.1

install:
	-cp ${OBJECT.d}/histui.1 /usr/local/share/man/man1/
	cp ${OUTPUT} ~/bin/

help:
	@echo Available targets:
	@echo "  histui"
	@echo "  docs"
	@echo "  install"
	@echo "  clean"
