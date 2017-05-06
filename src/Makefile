CC =gcc
CFLAGS =-W -Wall

DIR=..
BIN=$(DIR)/bin/
SRC=$(DIR)/src/
EXEC=exec

.PHONY: all clean

all: $(EXEC)

exec: outil.o
	$(CC) -o $(BIN)$@ $^

$(BIN)%: %.o
	@if [ -d $(BIN) ]; then : ; else mkdir $(BIN); fi
	$(CC) -o $(BIN)/$@ $^

%.o: $(SRC)%.c
	@if [ -d $(BIN) ]; then : ; else mkdir $(BIN); fi
	$(CC) -o $@ -c $<

clean:
	rm -f *.o
	rm -f $(BIN)/*
