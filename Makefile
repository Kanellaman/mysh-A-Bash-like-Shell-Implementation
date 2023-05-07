# Compiler
CC = gcc

#.c files
Files = mysh.c ShellFunctions.c History_Aliases.c Tokenize.c

#.o objects
OBJS = mysh.o ShellFunctions.o History_Aliases.o Tokenize.o

#Executable
EXEC = mysh

# Create executable
$(EXEC): $(OBJS) $(Files)
	$(CC) $(OBJS) -o $(EXEC)

# Memory-loss
memory:
	make
	valgrind --leak-check=full --track-origins=yes ./$(EXEC)

# Clear
clean:
	rm -f $(EXEC) $(OBJS)