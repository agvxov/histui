.PHONY: clean run

ifeq (${DEBUG}, 1)
  LFLAGS   += --debug --trace
  CXXFLAGS += -DDEBUG -O0 -ggdb -fno-inline	
  WRAP     := valgrind --track-origins=yes --leak-check=full --show-leak-kinds=all 
else
  CXXFLAGS += -O3 -fno-stack-protector -fno-exceptions -fno-rtti
endif

LIBFLAGS := $$(pkgconf --cflags ncurses readline sqlite3)
CXXFLAGS += -std=gnu++20 -I./source/ -I./object/ -I./ ${LIBFLAGS}
CXXFLAGS += -Wall -Wextra -Wpedantic 
LINKasd  += $$(pkgconf --libs ncurses readline sqlite3) 

OBJECT.d:=object/
SOURCE.d:=source/
SOURCE:=bash_history.yy.cpp main.cpp tui.cpp storage.cpp damerau_levenshtein.cpp
OBJECT:=$(addprefix ${OBJECT.d},$(addsuffix .o,$(basename ${SOURCE})))
SOURCE:=$(addprefix ${SOURCE.d},${SOURCE})

OUTPUT:=histui

${OUTPUT}: ${OBJECT}
	${LINK.cpp} ${OBJECT} ${LINKasd} -o ${OUTPUT}

object/%.yy.cpp: source/%.l
	${LEX} ${LFLAGS} --prefix=$*_ --header-file=$(basename $@).hpp -o $@ $<

object/%.o: object/%.l.cpp
	${COMPILE.cpp} $< -o $@

object/%.o: source/%.cpp
	${COMPILE.cpp} $< -o $@

clean:
	-rm ${OBJECT.d}/*
	-rm ./${OUTPUT}

test:
	${WRAP} ./${OUTPUT}
