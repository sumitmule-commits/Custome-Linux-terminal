TARGET = my_shell
OBJ = src/myshell.c src/input_parser.c src/helper.c src/built_in.c
CFLAGS = -Wall -Wextra -Werror
CC = gcc

all: $(TARGET)
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)
clean:
	rm -f *.o
fclean: clean
	rm -f $(TARGET)
re: fclean all