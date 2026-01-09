CC = gcc
CFLAGS = -Wall -Wextra -g -O0 -std=c23
SRC=src
OBJ=obj
SRCS=$(wildcard $(SRC)/*.c)
OBJS=$(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SRCS))
LINKER_FLAGS = -lSDL2 -lSDL2_image -lSDL2_mixer -lm
BIN = bin/shtmup

all:$(BIN)

release: CFLAGS=-Wall -O2 -DNDEBUG -std=c23
release: clean
release: $(BIN)

$(BIN): $(OBJS)
	@mkdir -p bin
	$(CC) $(CFLAGS) $(OBJS) $(LINKER_FLAGS) -o $@

$(OBJ)/%.o: $(SRC)/%.c
	@mkdir -p obj
	$(CC) $(CFLAGS) -c $< -o $@

run:
	./$(BIN)

clean:
	rm -rf bin obj