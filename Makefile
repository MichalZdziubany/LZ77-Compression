# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -O2

# Executable name
PROGRAM = Lz77

# Source file
SRC = Lz77.c

# Object file
OBJ = $(SRC:.c=.o)

# Build the program
$(PROGRAM): $(OBJ)
	$(CC) $(CFLAGS) -o $(PROGRAM) $(OBJ)

# Compile .c files into .o files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up compiled files
clean:
	del /F /Q $(PROGRAM) $(OBJ)

# Run the program
run: $(PROGRAM)
	./$(PROGRAM).exe