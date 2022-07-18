SOURCES=main.cpp
HEADERS= Timer.hpp Level.hpp Entity.hpp Vec2d.hpp Ncurses.hpp
FLAGS=-Wall -Wextra -Wfatal-errors  -lncursesw
APPNAME=a.out
DIR_INSTALL=~/bin

$(APPNAME): $(SOURCES) $(HEADERS)
	g++  -o $(APPNAME) $(SOURCES) $(FLAGS)


run: $(APPNAME)
	./$(APPNAME)

play: $(APPNAME)
	xterm ./$(APPNAME)

install: $(APPNAME) $(DIR_INSTALL)
	chmod +x $(APPNAME)
	cp $(APPNAME) -t $(DIR_INSTALL)


.PHONY: install