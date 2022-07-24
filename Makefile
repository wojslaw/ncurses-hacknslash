SOURCES=main.cpp entity.cpp vec2d.cpp ncurses.cpp timer.cpp baseentity.cpp level.cpp collisiontable.cpp
HEADERS= Timer.hpp Level.hpp Entity.hpp Vec2d.hpp Ncurses.hpp VisualEntity.hpp CollisionTable.hpp BaseEntity.hpp Ability.hpp
FLAGS=-Wall -Wextra -Wfatal-errors  -lncursesw -g1 -rdynamic -O0
APPNAME=ancof
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


.PHONY: install run play
