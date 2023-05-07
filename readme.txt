Assignment 1 - My Simple Shell (mysh)
sdi2000064-Konstantinos Kanellakis

Compilation/Execute

For compilation use command:
    make

For executing use command:
    ./mysh

Programming Design Choices

- Commands
Each command given to the shell is being tokenized where each element (command,redirections,pipes,special characters/symbols etc) corresponds to a different
token/string.
If were given multiple commands in a line, then we store the tokens of each command in a different column at the char*** tok array.

- Signals
In order for signals to work, every command that is running through exec() is being assigned to a process group. There has been used a job control philosophy. Foreground commands
that are running simutaneously such as pipeline, join the same process group and the pg leader is the first command of the pipeline. Background commands are categorized the same
but in a different pg. Every other command that is running/started alone is getting its own pg. When a fg command is terminated the fg variable which keep the pg leader is being
reset to 0.
The shell(main function) is handling the signals SIGINT and SIGSTP by sending them to the foreground processes that are currently running through the sig_handler function.
The shell, from the start, is also getting in a pg which contains only it. That way we give access to the terminal's read capability only to the pgs we want to, such as the 
foreground commands (when running and the shell is waiting for them) or the shell. We do not give the ability for bg processes to read from the terminal as it will interfer 
with the shell.

- Pipes
Every command that has the symbol | next to it, creates a pipe. Redirects its ouput to the pipe and its input (if its not the first command of the pipe). This happens repeatedly
till the last command of the pipe which we are not redirecting the output. After the pipeline is finished the input/output variables are reset to welcome the next pipeline.
If there is redirection in the pipeline the redirection has higher priority in case it interfers with a pips's input or output. Same as bash.

- Background processes
Background processes are not being waited from the shell but it checks periodically if any of them has finished. If so, a message is being printed.

- Redirection
If there are many redirections in a command,open all the files but read/write only from/to the last redirection. The same philosophy applies to bash too.

- Aliases
Only one word aliases are valid. There is support for an alias into an alias etc but if they start wtih the same token the search stops.
For example we have the command ls as an alias for "ls -l". The shell will replace ls with ls -l but will not search for further aliases of ls -l.
On the other hand if we have the alias "hd" for "fd" and the alias "fd" for "ls", if we run hd then ls will run.
If we import an alias which already exists then we replace its command to be an alias of with the new one given.

- Myhistory
History stores the last 20 commands. If the last command given is the same with the new one then we are not import it again. 
It can run any of the 20 commands (if exists) with the command myhistory <num>. There is also support in getting and executing aliases.

There are no memory leaks.