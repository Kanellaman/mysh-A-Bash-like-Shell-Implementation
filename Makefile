# Compiler
CC = gcc

#.c files
Files = mysh.c functions.c

#.o objects
OBJS = mysh.o functions.o

#Executable
EXEC = mysh

# Create Object files
$(OBJS): $(Files)
	$(CC) -c $(Files)
# Create executable
$(EXEC): $(OBJS)
	$(CC) $(OBJS) -o $(EXEC)
# Run
run: $(EXEC) $(EXEC2)
	./$(EXEC) 

# Memory-loss
memory:
	make
	valgrind --leak-check=full --track-origins=yes ./$(EXEC)

# https://stackoverflow.com/questions/5134891/how-do-i-use-valgrind-to-find-memory-leaks 

# Clear
clean:
	rm -f $(EXEC)