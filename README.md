# <div align="center"> Assignment 1-My Shell (mysh)<br/> Κ24 - System-Programming Semester 2022 - 2023

This is the README file for the implementation of "mysh" (My Simple Shell), a shell program written in C for Linux environments. It provides a user interface to create and manage processes, run system programs, and supports various features such as redirects, pipes, background execution, signal management, wild characters, and aliases. Bash commands and syntax are valid for this shell! Be careful as some complex syntaxes are not being supported.

## Contributing

Kanellakis Konstantinos

# Compile & Run

To compile the shell, execute the following command in the terminal:

`$ make`

To run the shell, execute the following command in the terminal:

`$ ./mysh`

Once the shell is launched, you will see the prompt:

`in-mysh-now:>`

You can then enter commands and interact with the shell.

# Features

## 1. Full support of Redirection (<,>,>>)

Run a program and save the output to a file:

```
in-mysh-now:> myProgram < file1 > file2
```

`myProgram` can be any bash command

```
in-mysh-now:> myProgram<file1>file2
```

## 2. Pipe Support (Both foreground and background)

You can use pipes to combine commands and perform operations on their outputs.

Example

```
in-mysh-now:> cat file1 file2 file3 | sort > file4
```

This command uses the output of cat command (files file1, file2, and file3) as input to the sort command, and the result is written to file4.

## 3. Execute Commands in the Background

You can run commands in the background using the & operator.

Example:

```
in-mysh-now:> sort file1 &; ls &;
```

Commands executed in the background run simultaneously with the next command. Background commands are not affected by signals Ctrl+C and Ctrl+Z.

## 4. Wild Characters Support

You can use wild characters to specify a subset of files in the current directory.

Example:

```
in-mysh-now:> ls file*.t?t
```

This command lists the files in the current directory that have a filename starting with "file", an extension that starts and ends with the character "t", and any one character in between.

## 5. Management of Aliases

You can create and destroy aliases for commands.

Example:

- Create an alias:

```
in-mysh-now:> createalias myhome "cd /home/users/smith"
```

This creates an alias called "myhome" that is equivalent to running the command cd /home/users/smith.

-Destroy an alias:

```
in-mysh-now:> destroyalias myhome
```

This deletes the alias "myhome".

## 6. Signals Management

The shell can handle simple signals.

-Ctrl+C: Sending a SIGINT signal to the running process via the mysh prompt. All foreground processes are terminated but not the mysh shell.

-Ctrl+Z: Sending a SIGSTP signal to the running process via the mysh prompt. All foreground processes are stopped but not the mysh shell.

## 7. Save History (myHistory)

The shell remembers the last 20 user commands and allows easy recall of the previously-run commands.
Example:

- This prints out the history of commands so far.

```
in-mysh-now:> myhistory
```

- This runs the 4th command of the history if exists

```
in-mysh-now:> myhistory 4
```

## Also supporting non spaces valid bash commands!

Examples:

```
in-mysh-now:> sort<file1>file2
in-mysh-now:> cat file1 file2|sort>file2
```

Other examples to run...

```
in-mysh-now:> ls ; cd ; ls
in-mysh-now:> myhistory 3 > file1
in-mysh-now:> createalias hd "ls -l"
in-mysh-now:> hd
in-mysh-now:> cd ; history 3 ; hd
in-mysh-now:> cat file1 ; sort file2 & sleep 10
```

# Brief summary of the code provided

1. The main function initializes variables and sets up signal handling using the signals function.
2. It enters a loop to continuously read user input until the user chooses to exit.
3. Inside the loop, it checks for any completed background processes using waitpid and prints a message for each completed process.
4. The input string is tokenized into individual commands using the tokenize function.
5. The commands are separated into a two-dimensional array of tokens using the separate function.
6. The code handles aliases and myhistory commands, replacing them with their respective commands.
7. Special commands like "exit" and "cd" are handled separately.
8. For each command in the two-dimensional array, it sets up input and output redirection if necessary, forks a child process, and executes the command using execvp.
9. Parent process handles foreground and background execution, waits for foreground processes to complete, and updates foreground process group.
10. The loop continues until the user chooses to exit.
