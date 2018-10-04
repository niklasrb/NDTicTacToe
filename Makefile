CC = g++

INCDIR = include
SRCDIR = src
OBJDIR = obj


CFLAGS=-Wall -Werror -pedantic -std=c++11  -g -O2 
LIBS = -L ../NeuralNetwork/lib -lnnetwork

_DEPS=  Position.h Grid.h Player.h Game.h HeuristicAI.h NeuralNetworkAI.h
DEPS=  TicTacToe.h $(patsubst %,$(INCDIR)/%,$(_DEPS)) 

_OBJ= Position.o Grid.o Player.o Game.o HeuristicAI.o NeuralNetworkAI.o
OBJ = $(patsubst %,$(OBJDIR)/%,$(_OBJ)) 

main: main.cpp $(DEPS) $(OBJ)
	$(CC) -o $@ $< $(OBJ) $(CFLAGS) $(LIBS)  

$(OBJ): $(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(DEPS)
	$(CC) -c -o $@ $<   $(CFLAGS)

.PHONY: clean
clean:
	-rm $(OBJ) main


