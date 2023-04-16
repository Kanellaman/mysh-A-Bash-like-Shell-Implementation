#Compiler
CC = gcc

#.c files
Files = mysh.c functions.c

#.o Files
OBJS = mysh.o functions.o

#Executable
EXEC = mysh

#Parameters for run
ARGS = 3 12 12 6

#Create executable
$(EXEC): $(Files)
	$(CC) -o $(EXEC) mysh.c functions.c

#Run
run: $(EXEC) $(EXEC2)
	./$(EXEC) 

#debug
debug: 
	$(CC) -g3 $(Files) -o $(EXEC) 
	gdb -q $(EXEC)

#memory-loss
memory:
	make
	valgrind --leak-check=full ./$(EXEC)

#https://stackoverflow.com/questions/5134891/how-do-i-use-valgrind-to-find-memory-leaks 

#Clear
clean:
	rm -f $(EXEC) 	