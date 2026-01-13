.PHONY: clean test
.SUFFIXES:

# --- Paths / files
SOURCE.d := source/
OBJECT.d := object/

SOURCE := main.c cli.c tui.c storage.c
OBJECT := ${SOURCE}
OBJECT := $(subst .c,.o,${OBJECT})

GENSOURCE := argument_yy.tab.c

vpath %.o ${OBJECT.d}
vpath %.c ${SOURCE.d}
vpath %.l ${SOURCE.d}/bash/
vpath %.l ${SOURCE.d}/fish/
vpath %.y ${SOURCE.d}
vpath %.yy.c ${OBJECT.d}
vpath %.tab.c ${OBJECT.d}

OUT := histui

# --- Tools/Flags
ifeq (${DEBUG}, 1)
  CPPFLAGS += -DDEBUG

  CFLAGS.D += -Wall -Wextra -Wpedantic
  CFLAGS.D += -O0 -ggdb -fno-inline
  CFLAGS.D += -fsanitize=address,undefined
  CFLAGS   += ${CFLAGS.D}

  WRAP     := valgrind --track-origins=yes --leak-check=full --show-leak-kinds=all 
else
  CFLAGS += -O3 -flto=auto
endif

TARGET_SHELL ?= bash

ifeq (${TARGET_SHELL}, bash)
    GENSOURCE += bash_history.yy.c
else ifeq (${TARGET_SHELL}, fish)
    GENSOURCE += fish_history.yy.c
endif

CPPFLAGS += -DSHELL=${TARGET_SHELL} -Isource/${TARGET_SHELL}
CPPFLAGS += -I${SOURCE.d} -I${OBJECT.d}
LIBFLAGS := $$(pkgconf --cflags ncurses readline sqlite3)
CFLAGS   += -std=c2x ${LIBFLAGS}
CFLAGS   += -Wall -Wextra -Wpedantic 
LDLIBS   += $$(pkgconf --libs ncurses readline sqlite3)

OUT:=histui

# --- Rule Section ---
all: ${OUT}

${OUT}: ${GENSOURCE} ${OBJECT}
	${LINK.c} -o $@ $(addprefix ${OBJECT.d}/,${OBJECT} ${GENSOURCE}) ${LDLIBS}

%.o: %.c
	${COMPILE.c} -o ${OBJECT.d}/$@ $<

%.yy.c: %.l
	flex --prefix=$*_ -o ${OBJECT.d}/$@ --header=${OBJECT.d}/$(subst .c,.h,$@) $? 

%.tab.c: %.y
	bison --name-prefix=$*_ -o ${OBJECT.d}/$@ --header=${OBJECT.d}/$(subst .c,.h,$@) $? 

%.yy.o: %.yy.c
	${COMPILE.c} -o ${OBJECT.d}/$@ ${OBJECT.d}/$<

%.tab.o: %.tab.c
	${COMPILE.c} -o ${OBJECT.d}/$@ ${OBJECT.d}/$<

clean:
	-${RM} $(or ${OBJECT.d},#)/*
	-${RM} ./${OUT}

test:
	${WRAP} ./${OUT}

docs:
	kramdown-man documentation/histui.1.md -o ${OBJECT.d}/histui.1

install:
	-cp ${OBJECT.d}/histui.1 /usr/local/share/man/man1/
	cp ${OUT} ~/bin/

help:
	@echo Available targets:
	@echo "  histui"
	@echo "  docs"
	@echo "  install"
	@echo "  clean"
