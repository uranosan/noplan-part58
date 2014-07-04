PROGRAM		= a.exe
CC 			= gcc
# FLAGS		= -std=c99 -Wall
FLAGS		= -std=c11 -Wall

SOURCE		= Main.c GameMain.c Common.c Console.c KeyInput.c UI.c \
                  Map.c Human.c Player.c Npc.c Hiroin.c Zombie.c \
                  Effect.c \
                  List.c Maze.c \
                  Manager.c Factory.c

HEADER		= $(SOURCE:%.c=%.h)
OBJS		= $(SOURCE:%.c=%.o)

$(PROGRAM) : $(OBJS)
	$(CC) $(FLAGS) $(OBJS) -o $(PROGRAM)

Main.o     : Main.c GameMain.h
	$(CC) $(FLAGS) -c Main.c

GameMain.o : GameMain.c GameMain.h $(HEADER)
	$(CC) $(FLAGS) -c GameMain.c

Manager.o : Manager.c Manager.h Factory.h Map.h Maze.h
	$(CC) $(FLAGS) -c Manager.c

Factory.o : Factory.c Factory.h Human.h Player.h Zombie.h Hiroin.h
	$(CC) $(FLAGS) -c Factory.c

Common.o   : Common.c Common.h
	$(CC) $(FLAGS) -c Common.c

Console.o  : Console.c Console.h
	$(CC) $(FLAGS) -c Console.c

KeyInput.o : KeyInput.c KeyInput.h
	$(CC) $(FLAGS) -c KeyInput.c

UI.o       : UI.c UI.h Common.h Console.h KeyInput.h Human.h Player.h
	$(CC) $(FLAGS) -c UI.c

Map.o      : Map.c Map.h Common.h Console.h Maze.h
	$(CC) $(FLAGS) -c Map.c

Maze.o     : Maze.c Maze.h Common.h Map.h
	$(CC) $(FLAGS) -c Maze.c

Human.o    : Human.c Human.h Common.h Console.h Map.h Effect.h
	$(CC) $(FLAGS) -c Human.c

Player.o   : Player.c Player.h Common.h Map.h Human.h KeyInput.h
	$(CC) $(FLAGS) -c Player.c

Npc.o      : Npc.c Npc.h Common.h Human.h Player.h
	$(CC) $(FLAGS) -c Npc.c

Hiroin.o   : Hiroin.c Hiroin.h Common.h Human.h Npc.h Player.h
	$(CC) $(FLAGS) -c Hiroin.c

Zombie.o   : Zombie.c Zombie.h Common.h Human.h Npc.h Player.h
	$(CC) $(FLAGS) -c Zombie.c

Effect.o   : Effect.c Effect.h Common.h Console.h List.h
	$(CC) $(FLAGS) -c Effect.c

List.o     : List.c List.h
	$(CC) $(FLAGS) -c List.c 

all        :
	$(CC) $(FLAGS) $(SOURCE) -o $(PROGRAM)

check      :
	$(CC) $(FLAGS) -Wall -fsyntax-only $(SOURCE)

clean      :
	rm -f $(OBJS) $(PROGRAM)
