CC=cc
BIN=cpuc
SRC=cpuc.c
FLAGS=-s -Os

.PHONY: all
all:
	$(CC) $(SRC) $(FLAGS) -o $(BIN)

.PHONY: clean
clean:
	rm -f *.o $(BIN)

