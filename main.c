#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

#define BUFFER_SIZE 1024
#define TOK_BUFSIZE 64
#define TOK_DELIM " \t\r\n\a"

int cd_program(char **args);
int help_program(char **args);
int exit_program(char **args);

char *available_programas[] = {
    "cd",
    "help",
    "exit"};

int (*programs[])(char **) = {
    &cd_program,
    &help_program,
    &exit_program,
};

int available_programs_size()
{
    return sizeof(available_programas) / sizeof(char *);
}

int cd_program(char **args)
{
    if (args[1] == NULL)
    {
        perror("ARG NULL");
    }
    else
    {
        if (chdir(args[1]) != 0)
        {
            perror("bro");
        };
    };
    return 1;
};

int help_program()
{
    int i;
    printf("Den's SHELL\n");
    printf("Type program names and arguments, and hit enter.\n");
    printf("The following are built in:\n");

    for (i = 0; i < sizeof(available_programas) / sizeof(char *); i++)
    {
        printf("  %s\n", available_programas[i]);
    }

    printf("Use the man command for information on other programs.\n");
    return 1;
};

int exit_program(char **args)
{
    return 0;
}

int command_start(char **args)
{
    pid_t pid, wpid;
    int status;

    pid = fork();
    if (pid == 0)
    {
        if (execvp(args[0], args) == -1)
        {
            perror("ERROR");
        }
        exit(EXIT_FAILURE);
    }
    else if (pid < 0)
    {
        perror("ERROR");
    }
    else
    {
        do
        {
            waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
}

char **split_line(char *line)
{
    int buffsize = TOK_BUFSIZE;
    char **tokens = malloc(sizeof(char) * buffsize);
    char *token;
    int position = 0;

    if (!tokens)
    {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, TOK_DELIM);
    while (token != NULL)
    {
        tokens[position] = token;
        position++;
        if (position >= buffsize)
        {
            buffsize = buffsize + TOK_BUFSIZE;
            tokens = realloc(tokens, buffsize * sizeof(char));
            if (!tokens)
            {
                fprintf(stderr, "lsh: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
        token = strtok(NULL, TOK_DELIM);
    }
    tokens[position] = NULL;
    return tokens;
}

// getline() approach
char *read_line_getline()
{
    int *line = NULL;
    ssize_t bufsize = 0;
    if (getline(&line, &bufsize, stdin) == -1)
    {
        if (feof(stdin))
        {
            exit(EXIT_SUCCESS);
        }
        else
        {
            exit(EXIT_FAILURE);
        }
    }
    return line;
}

// getchar() approach
char *read_line()
{
    int position = 0;
    int buffer_size = BUFFER_SIZE;
    char *buffer = malloc(sizeof(char) * buffer_size);
    int c;

    if (!buffer)
    {
        fprintf(stderr, "BUFFER ERROR\n");
        exit(EXIT_FAILURE);
    }
    while (1)
    {
        c = getchar();
        if (c == EOF || c == '\n')
        {
            buffer[position] = '\0';
            return buffer;
        }
        else
        {
            buffer[position] = c;
        }
        position++;

        if (position >= buffer_size)
        {
            buffer_size = buffer_size + BUFFER_SIZE;
            buffer = realloc(buffer, buffer_size);
            if (!buffer)
            {
                fprintf(stderr, "BUFFER ERROR\n");
                exit(EXIT_FAILURE);
            }
        }
    }
}

int execute(char **args)
{
    int i;

    if (args[0] == NULL)
    {
        return 1;
    }

    for (i = 0; i < sizeof(available_programas) / sizeof(char *); i++)
    {
        if (strcmp(args[0], available_programas[i]) == 0)
        {
            return (*programs[i])(args);
        }
    }

    return command_start(args);
}

void loop()
{
    int *line;
    int **args;
    int status;
    FILE *res;

    res = fopen("res.txt", "w+");

    do
    {
        printf("> ");
        line = read_line();
        args = split_line(line);
        status = execute(args);

        free(line);
        free(args);

    } while (status);
}

int main()
{
    loop();
}