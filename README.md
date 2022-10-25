# shell-in-C
I create a Shell in C from scratch that does most of the things you would need a shell to do.

# Creating a shell in C

A shell needs to do 3 things in loop:

- read a line
- separate it with spaces
- execute them

---

### READING A LINE
There are 2 different ways of doing so: with a [getline()]("https://c-for-dummies.com/blog/?p=1112") (newer and easier approach) or with the getchar() with implies allocating memory dinamically so that when you exceed it, you don't stop

**getchat() approach**:

- allocate memory (start with 1024) creating a buffer in a malloc
- checks for the buffer
- getchar() in an infinite loop
- check if EOF or \n, then return buffer, else just add it (use a position variable with the buffer)
- if you eceed memory, allocate more with realloc

**[getline()]("https://c-for-dummies.com/blog/?p=1112") approach**:

- use a ssize_t buffer so that [getline()]("https://c-for-dummies.com/blog/?p=1112") can take care of that
- [getline()]("https://c-for-dummies.com/blog/?p=1112") takes 3 arguments:
  - a pointer for your line
  - an ssize_t buffer
  - stdin
- check [getline()]("https://c-for-dummies.com/blog/?p=1112") return, and check if it was becasue of EOF or unknown

### SPLITTING THE LINE

I’m going to make a glaring simplification here, and say that we won’t allow quoting or backslash escaping in our command line arguments. Instead, we will simply use whitespace to separate arguments from each other. So the command echo "this message" would not call echo with a single argument this message, but rather it would call echo with two arguments: "this **and** message".

In order the "tokenize" the string after a whitespace we are going to use the [strtock]("https://www.tutorialspoint.com/c_standard_library/c_function_strtok.htm") c library
Let's get started:

- We need to define a buffer size for the array of commands we are going to have
- Define a token delim for the space (i use "\t\r\n\a")
- get the token using the library continuosly until we get them all, and add each one in an array
- check if the buffer exceeded

### RUNNING OUR PROGRAMS

---

##### How processes work in unix
Now, we’re really at the heart of what a shell does. Starting processes is the main function of shells. So writing a shell means that you need to know exactly what’s going on with processes and how they start. That’s why I’m going to take us on a short diversion to discuss processes in Unix-like operating systems.

There are only two ways of starting processes on Unix. The first one (which almost doesn’t count) is by being Init. You see, when a Unix computer boots, its kernel is loaded. Once it is loaded and initialized, the kernel starts only one process, which is called Init. This process runs for the entire length of time that the computer is on, and it manages loading up the rest of the processes that you need for your computer to be useful.

Since most programs aren’t Init, that leaves only one practical way for processes to get started: the fork() system call. When this function is called, the operating system makes a duplicate of the process and starts them both running. The original process is called the “parent”, and the new one is called the “child”. fork() returns 0 to the child process, and it returns to the parent the process ID number (PID) of its child. In essence, this means that the only way for new processes is to start is by an existing one duplicating itself.

This might sound like a problem. Typically, when you want to run a new process, you don’t just want another copy of the same program – you want to run a different program. That’s what the exec() system call is all about. It replaces the current running program with an entirely new one. This means that when you call exec, the operating system stops your process, loads up the new program, and starts that one in its place. A process never returns from an exec() call (unless there’s an error).

With these two system calls, we have the building blocks for how most programs are run on Unix. First, an existing process forks itself into two separate ones. Then, the child uses exec() to replace itself with a new program. The parent process can continue doing other things, and it can even keep tabs on its children, using the system call wait().

---

##### What to code:

- we first need to [fork()]("https://www.geeksforgeeks.org/fork-system-call/")
- we need to then execute, with [execvp()]("https://www.digitalocean.com/community/tutorials/execvp-function-c-plus-plus") (or any other exec tbh, choice up to you)
- If the exec command returns -1 (or actually, if it returns at all), we know there was an error. So, we use [perror()]("https://www.tutorialspoint.com/c_standard_library/c_function_perror.htm") to print the system’s error message
- we need to check if the process returns -1, and deal with the error
- finally we can run the program. we use [waitpid()]("https://www.ibm.com/docs/en/zos/2.3.0?topic=functions-waitpid-wait-specific-child-process-end") to wait for the process’s state to change. Unfortunately, waitpid() has a lot of options (like exec()). Processes can change state in lots of ways, and not all of them mean that the process has ended. A process can either exit (normally, or with an error code), or it can be killed by a signal. So, we use the macros provided with waitpid() to wait until either the processes are exited or killed. Then, the function finally returns a 1, as a signal to the calling function that we should prompt for input again.

### CREATE BUILT IN COMMANDS
Most commands a shell executes are programs, but not all of them. Some of them are built right into the shell. So, it makes sense that we need to add some commands to the shell itself. The ones I added to my shell are _cd_, _exit_, and _help_
So let's start:

- let's start by creating an array with all the program names, so it's easier to chose which one to execute later
- now let's create a list of functions to chose from like this:

```
int (*builtin_func[]) (char **) = {
  &lsh_cd,
  &lsh_help,
  &lsh_exit
};
```

- we can now start creating the cd function:
  - let's check that the fisrt argument is not null
  - let's use [chdir()]("https://www.geeksforgeeks.org/chdir-in-c-language-with-examples/"), and check for 0 values (errors) and deal with perror()
- now let's move to the help function:
  - This is just going to display a list of commands you can execute
- exit function:
  - returns 0 (what did you expect?)

### FINALLY EXECUTE
what's now left to do is simply chose between the functions based on what the first argument is
