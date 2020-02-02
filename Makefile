CC=g++
PROG=bezier
CLIBS=-lSDL2
OBJS=main.o

all : $(OBJS)
	$(CC) $(OBJS) -o $(PROG) $(CLIBS)

%.o : $*.cpp $*.hpp
	$(CC) -c $^ -O3 $(CLIBS)
