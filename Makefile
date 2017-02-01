CC=g++
debug=-g

all : rrabbit.o rgame.o
	$(CC) $(debug) rrabbit.o rgame.o creatures.o `wx-config --libs` -o rrabbit

rrabbit.o : rrabbit.cpp rgame.o
	$(CC) $(debug) -c rrabbit.cpp `wx-config --cflags` -o rrabbit.o

rgame.o : rgame.cpp rgame.h creatures.o
	$(CC) $(debug) -c rgame.cpp `wx-config --cflags` -o rgame.o

creatures.o : creatures.cpp creatures.h
	$(CC) $(debug) -c creatures.cpp `wx-config --cflags` -o creatures.o

.PHONY : clean
clean :
	rm *.o
	
.PHONY : install
install :
	if !([ -e /usr/games ]) then mkdir /usr/games;fi
	if !([ -e /usr/games/rrabbit ]) then mkdir /usr/games/rrabbit;fi
	cp -rv ./* /usr/games/rrabbit/
	chmod 777 /usr/games/rrabbit
	chmod 777 /usr/games/rrabbit/ScoreS
	if !([ -e /usr/bin/rrabbit ]) then ln -s /usr/games/rrabbit/rrabbit /usr/bin/rrabbit;fi
	
	
