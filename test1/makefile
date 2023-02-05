# Použitý překladač
CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99 -pthread

# Binární soubory projektu
BIN = main.exe
OBJ = main.o commands.o filesystem.o parse_path.o

# Návody pro sestavení projektu
$(BIN): $(OBJ)
	$(CC) $(OBJ) -o $(BIN) $(CFLAGS)

main.o: main.c
	gcc -c main.c $(CFLAGS)

commands.o: commands.c commands.h
	gcc -c commands.c $(CFLAGS)

filesystem.o: filesystem.c filesystem.h
	gcc -c filesystem.c $(CFLAGS)

parse_path.o: parse_path.c parse_path.h
	gcc -c parse_path.c $(CFLAGS)

clean:
	del -f $(BIN) $(OBJ)