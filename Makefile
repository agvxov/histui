.PHONY: clean run

ifeq (${DEBUG}, 1)
  LFLAGS   += --debug --trace
  CXXFLAGS += -Wall -Wextra -Wpedantic 
  CXXFLAGS += -DDEBUG -O0 -ggdb -fno-inline	
  WRAP     := valgrind --track-origins=yes --leak-check=full --show-leak-kinds=all 
else
  CXXFLAGS += -O3 -fno-stack-protector -fno-exceptions -fno-rtti
endif

LDLIBS   += $$(pkgconf --cflags --libs ncurses) $$(pkgconf --cflags --libs readline)
CXXFLAGS += -std=gnu++20 -I./source/ -I./object/ -I./

OBJECT.d:=object/
SOURCE.d:=source/
SOURCE:=main.cpp tui.cpp db.cpp config.l
SOURCE:=$(addprefix ${SOURCE.d},${SOURCE})
OBJECT:=$(addprefix ${OBJECT.d},$(subst ${SOURCE.d},,$(addsuffix .o,$(basename ${SOURCE}))))

OUTPUT:=histui

${OUTPUT}: ${OBJECT}
	${LINK.cpp} ${OBJECT} -o ${OUTPUT} ${LDLIBS}

object/%.l.cpp: source/%.l
	${LEX} ${LFLAGS} -o $@ $<

object/%.o: object/%.l.cpp
	${COMPILE.cpp} $< -o $@

object/%.o: source/%.cpp
	${COMPILE.cpp} $< -o $@

clean:
	-rm ${OBJECT.d}/*
	-rm ./${OUTPUT}

run:
	./${OUTPUT}
