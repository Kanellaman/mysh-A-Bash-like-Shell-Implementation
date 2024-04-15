# Compiler
CC = gcc

#.c files
Files = src/mysh.c Modules/ShellFunctions.c Modules/History_Aliases.c Modules/Tokenize.c

#.o objects
OBJS = src/mysh.o Modules/ShellFunctions.o Modules/History_Aliases.o Modules/Tokenize.o

#Executable
EXEC = mysh

# Create executable
$(EXEC): $(OBJS) $(Files)
	$(CC) $(OBJS) -o $(EXEC)

# Clear
clean:
	rm -f $(EXEC) $(OBJS)