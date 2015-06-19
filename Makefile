BIN=snake
LDFLAGS=-lSDL2
CFLAGS=-std=c99 -pedantic
RM=rm -f

all: $(BIN)

$(BIN):

clean:
	$(RM) $(BIN)
