# MicroShell
Implemented the Unix shell. *ush* is a command interpreter with a syntax similar to UNIX C shell, csh.  However, it is for instructional purposes only, therefore it is much simpler.
### Initialization and Termination
When first  stared, ush normally  performs  commands  from  the file ̃/.ushrc, provided  that  it  is  readable. Commands in this  file are processed just the same as if they were taken from standard input.
### Interactive Operation
After startup processing, an interactive ush shell begins reading commands from the terminal, prompting with *hostname%*. The shell then repeatedly performs the following actions: a line of command input is read and broken into words; this sequence of words is parsed; and the shell executes each command in the current line.
## Working
### Lexical Structure
The shell splits input lines into words separated by whitespace (spaces or tabs), with the following exceptions:
* The special characters &, |, ;, <, and > and the multi-character sequences >>, |&, >& and >>& are always separate words, whether or not they are surrounded by whitespace.
* Special characters preceded by a backslash character (\) are not interpreted by the shell. Two backslashes together sends the backslash character to the shell; otherwise the backslash is stripped from the input.
* Characters enclosed in double quotes (") or single quotes (’) form a single word.  Special characters inside of strings do not form separate words.

### Command Line Parsing
A *simple  command* is a sequence of words, the first of which specifies the command to be executed.  A pipeline is a sequence of one or more simple commands separated by | or |&. With |, the standard output of the preceding command is redirected to the standard input of the command that follows.  With |&, both the standard error and the standard output are redirected through the pipeline.

A *list* is a sequence of one or more pipelines separated by ; or &. Pipelines joined into sequences with ; will be executed sequentially. Pipelines ending with & are executed asynchronously. In which case, the shell does not wait for the pipeline to  finish; instead, it displays the job number and associated process ID, and begins processing the subsequent pipelines (prompting if necessary).

### I/O Redirection
The following separators indicate that the subsequent word is the name of a file to which the command’s standard input, standard output, or standard error is redirected.
* < Redirect the standard input.
* >, >& Redirect the standard output to a file. If file does not exist, it is created. If it does exist, it is over-
written and its previous contents are lost. The & form redirects both standard output and standard
error to the file.
* >>, >>& Append the standard output. Like >, but places output at the end of the file rather than overwriting.  The & form appends both standard output and standard error to the file.

### Command Execution
If the command is an ush shell built-in, the shell executes it directly. Otherwise, the shell searches for a file by that name with execute access. If the command-name contains a /, the shell takes it as a pathname and searches for it. If a pathname begins with a /, then the path is absolute; otherwise, the path is relative to the current working directory. If the command-name does not contain a /, the shell attempts to resolve it to a pathname, searching each directory in the PATH variable for the command.

When a file, with its pathname resolved, is found that has proper execute permission, the shell forks a new process to run the command. The shell also passes along any arguments to the command. Using one of the flavor of exec system call, such as execv(2V), the newly forked process attempts to overlay the desired program.  If successful, the shell is silent.

If the file does not have execute permissions, or if the pathname matches a directory, a ‘‘permission denied’’ message is displayed. If the pathname cannot be resolved a ‘‘command not found’’ message is displayed. If either of these errors occurs with any component of a pipeline the entire pipeline is aborted, even though some of the pipeline may already be executing.
### Environment Variables
Environment variables may be accessed via the *setenv* built-in commands. When a program is exec’d the environment variables are passed as parameters to execv or equivalent.
### Builtin commands
Built-in commands are executed within ush. If a built-in command occurs as any component of a pipeline except the last, it is executed in a subshell.
* **cd[dir]**  Change the working directory of the shell to dir, provided it is a directory and the shell has the appropriate permissions. Without an argument, it changes the working directory to the original (home) directory.
* **echo word...** Writes each word to the shell’s standard output, separated by spaces and terminated with a new-line.
* **logout** Exits the shell.
* **pwd** Prints the current working directory.
* **setenv[VAR[word]]** Without arguments, prints the names and values of all environment variables.  Given VAR, sets the environment variable VAR to word or, without word, to the null string.
* **unsetenv VAR** Removes environment variable whose name matches VAR.
* **where** command Reports  all known instances of command, including builtins and executables in path.

## Invocation
Clone or Download the repositry, Run the Makefile:
```
Make
```
Makefile is designed in such a way that it will create the sufficient files required to make the program work. Now, running the program:
```
./ush < test.1 > test.1.out
```
