CC = gcc
CFLAGS  = -g -Wall -Werror -Wextra -pedantic

SOURCE = main
TARGET = program

all: $(TARGET)

$(TARGET): $(SOURCE).c
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCE).c

clean:
	$(RM) $(TARGET)

run:
	./$(TARGET)
