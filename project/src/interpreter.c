#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shellmemory.h"
#include "shell.h"
#include <dirent.h>
#include "interpreter.h"
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <ctype.h>
#include "pcb.h"

#include "scheduler.h"

// int MAX_ARGS_SIZE = 3;

int badcommand () {
    printf ("Unknown Command\n");
    return 1;
}
int badcommandexec () {
    printf ("Bad command: exec\n");
    return 1;
}

int badcommandFileExist (char *operation, char *var) {
    printf ("my_mkdir: %s : already exists\n", var);
    return 1;
}

// For source command only
int badcommandFileDoesNotExist () {
    printf ("Bad command: File not found\n");
    return 1;
}
int badcommandmy_cd () {
    printf ("Bad command: my_cd\n");
    return 1;
}
int badcommandmy_mkdir () {
    printf ("Bad command: my_mkdir\n");
    return 1;
}
int badcommandmy_touch () {
    printf ("Bad command: my_touch\n");
    return 1;
}
int badcommandrun () {
    printf ("Bad command: run\n");
    return 1;
}

int str_isalphanum (char *name) {
    for (char c = *name; c != '\0'; c = *++name) {
        if (!(isdigit (c) || isalpha (c)))
            return 0;
    }
    return 1;
}



// Interpret commands and their arguments
int interpreter (char *command_args[], int args_size) {
    int i;

    if (args_size < 1) {
        return badcommand ();
    }

    for (i = 0; i < args_size; i++) {   // terminate args at newlines
        command_args[i][strcspn (command_args[i], "\r\n")] = 0;
    }

    if (strcmp (command_args[0], "help") == 0) {
        //help
        if (args_size != 1)
            return badcommand ();
        return help ();

    } else if (strcmp (command_args[0], "quit") == 0) {
        //quit
        if (args_size != 1)
            return badcommand ();
        return quit ();

    } else if (strcmp (command_args[0], "set") == 0) {
        //set
        if (args_size != 3)
            return badcommand ();
        return set (command_args[1], command_args[2]);

    } else if (strcmp (command_args[0], "print") == 0) {
        if (args_size != 2)
            return badcommand ();
        return print (command_args[1]);

    } else if (strcmp (command_args[0], "source") == 0) {
        if (args_size != 2)
            return badcommand ();
        return source (command_args[1]);

    } else if (strcmp (command_args[0], "echo") == 0) {
        // if there's no argument just print a new line
        if (args_size < 2) {
            printf ("\n");
            return 0;
        } else {
            // printf("args size = %d\n", args_size);
            return echo (command_args + 1, args_size - 1);
        }
    } else if (strcmp (command_args[0], "my_ls") == 0) {
        return my_ls ();
    } else if (strcmp (command_args[0], "my_mkdir") == 0) {
        if (args_size != 2) {
            return badcommandmy_mkdir ();
        }
        return my_mkdir (command_args[1]);
    } else if (strcmp (command_args[0], "my_cd") == 0) {
        if (args_size != 2) {
            return badcommandmy_cd ();
        }
        return my_cd (command_args[1]);
    } else if (strcmp (command_args[0], "my_touch") == 0) {
        if (args_size != 2) {
            return badcommandmy_touch ();
        }
        return my_touch (command_args[1]);
    } else if (strcmp (command_args[0], "my_pwd") == 0) {
        return my_pwd ();
    } else if (strcmp (command_args[0], "run") == 0) {
        // printf("args size = %d\n", args_size);
        if (args_size < 2) {
            return badcommandrun ();
        }
        return run (command_args + 1, args_size - 1);
    } else if (strcmp (command_args[0], "exec") == 0) {
        if (args_size > 6) {
            return badcommandexec ();
        }
        return exec (command_args + 1, args_size - 1);
    }

    else {
        return badcommand ();
    }
}

int help () {

    // note the literal tab characters here for alignment
    char help_string[] = "COMMAND			DESCRIPTION\n \
run             run a program\n \
my_mkdir STRING        make a directory\n \
my_cd  STRING         change directory\n \
my_touch  STRING       create a file\n \
my_ls               list the contents of the current directory\n \
my_pwd              print the current working directory\n \
echo STRING         print a string\n \
help			Displays all the commands\n \
quit			Exits / terminates the shell with “Bye!”\n \
set VAR STRING		Assigns a value to shell memory\n \
print VAR		Displays the STRING assigned to VAR\n \
source SCRIPT.TXT	Executes the file SCRIPT.TXT\n ";
    printf ("%s\n", help_string);
    return 0;
}

int quit () {
    printf ("Bye!\n");
    exit (0);
}

int set (char *var, char *value) {
    char *link = " ";

    // Hint: If "value" contains multiple tokens, you'll need to loop through them, 
    // concatenate each token to the buffer, and handle spacing appropriately. 
    // Investigate how `strcat` works and how you can use it effectively here.

    mem_set_value (var, value);
    return 0;
}

int run (char *command_args[], int args_size) {
    char **commands = calloc (args_size + 1, sizeof (char *));
    for (int i = 0; i < args_size; ++i) {
        commands[i] = command_args[i];
    }
    pid_t id = fork ();
    if (id < 0) {
        perror ("fork() failed");
        return 1;
    } else if (id == 0) {
        //Child process
        execvp (commands[0], commands);
        perror ("exec failed");
        exit (1);

    } else if (id > 0) {
        //Parent process
        waitpid (id, NULL, 0);
    }
    return 0;
}

int my_touch (char *var) {
    if (access (var, F_OK) != -1) {
        return badcommandFileExist ("my_touch", var);
    }
    int fd = creat (var, 0644);
    close (fd);
    return 0;
}
int my_pwd () {
    char pwd[256];
    getcwd (pwd, sizeof (pwd));
    printf ("%s\n", pwd);
    return 0;
}

int my_ls () {
    struct dirent **list;

    int n;
    n = scandir (".", &list, 0, alphasort);
    for (int i = 0; i < n; i++) {
        printf ("%s\n", list[i]->d_name);
    }
    free (list);
    return 0;


}
int my_mkdir (char *var) {
    char *pathname = var;
    int must_free = 0;
    if (*pathname == '$') {
        // lookup name
        pathname = mem_get_value (pathname + 1);
        if (pathname) {
            // name exists, should free whatever we got
            must_free = 1;
        }
    }
    // If pathname doesn't exist or is invalid, return an error
    if (!pathname || !str_isalphanum (pathname)) {
        if (must_free)
            free (pathname);
        return badcommandmy_mkdir ();
    }

    if (mkdir (pathname, 0777) != 0) {
        return badcommandmy_mkdir ();
    }
    if (must_free) {
        free (pathname);
    }
    return 0;
}
int my_cd (char *path) {
    if (chdir (path) != 0) {
        return badcommandmy_cd ();
    }
    return 0;
}

int echo (char *command_args[], int args_size) {
    for (int i = 0; i < args_size; i++) {
        char first_char = command_args[i][0];
        if (first_char == '$') {
            char *value = mem_get_value (command_args[i] + 1);
            if (value == NULL) {
                printf ("variable does not exist\n");
            } else {
                printf ("%s", value);
            }

        } else {
            printf ("%s", command_args[i]);
        }
        if (i < args_size - 1) {
            printf (" ");
        }
    }
    printf ("\n");
    return 0;
}


int print (char *var) {
    char *s = mem_get_value (var);
    if (s == NULL) {
        printf ("Variable does not exist\n");
        return 1;
    }
    printf ("%s\n", s);
    return 0;
}

int source (char *script) {
    char *a[] = { script, "FCFS" };
    exec (a, 2);

    return 0;
}

//exec helper

int exec (char *args[], int args_size) {
    queue_init (&readyQueue);

    if (args_size < 2) {
        return badcommandexec ();
    }
    // printf("args_size: %d\n", args_size);
    int scripts;
    enum POLICIES policy;
    int backgroundMode = strcmp (args[args_size - 1], "#") == 0;
    int policyIndex = args_size - (backgroundMode ? 2 : 1);
    scripts = policyIndex;
    // printf("total scripts: %d\n", scripts);
    policy = get_policy (args[policyIndex]);

    if (policy == INVALID) {
        return badcommandexec ();
    }
    char line[MAX_LINE_LENGTH];
    for (int i = 0; i < scripts; i++) {
        script_add_script (args[i]);
    }
    // Schedule the scripts
    PCB **pcbArray = create_pcb_array (scripts);
    switch (policy) {
        case FCFS:
        case RR:
        case RR30:
            schedule_no_sort (&readyQueue, pcbArray, scripts);
            break;
        case SJF:
        case AGING:
            schedule_sort (&readyQueue, pcbArray, scripts);
            break;
    }

    if (backgroundMode) {
        allocate_batch_script ();
    }
    // printf("current total script after : %ld\n", scriptmemory.totalScripts);
    // print_script();
    // print_queue (&readyQueue);
    // exit(0);

    switch (policy) {
        case RR:
            execute_RR (&readyQueue, 2);
            break;
        case RR30:
            execute_RR (&readyQueue, 30);
            break;
        case FCFS:
        case SJF:
            execute_non_preemptive (&readyQueue);
            break;
        case AGING:
            execute_AGING (&readyQueue);
            break;
    }
    // print_script();
    return 0;
}

void extract_source (char *line, int current_scripts) {
    char *token = strtok (line, " ");
    current_scripts++;

    int argc = 0;
    char *argv[64];
    while (token != NULL) {
        argv[argc++] = token;
        token = strtok (NULL, " ");
    }
    // printf("adding next script id to be : %d\n", current_scripts);
    for (int i = 1; i < argc - 1; i++) {
        script_load_script (current_scripts, argv[i]);
        scriptmemory.totalScripts++;
        PCB *pcb = malloc (sizeof (PCB));
        pcb_init (pcb);
        pcb->process_index = scriptmemory.totalScripts;
        enqueue (&readyQueue, pcb);
    }
}

/// "#" tag helper

void allocate_batch_script () {
    char line[MAX_LINE_LENGTH];
    int current_scripts = scriptmemory.totalScripts;
    while (fgets (line, sizeof (line), stdin) != NULL) {
        if (strcmp (line, "#\n") == 0) {
            break;
        }
        if (strncmp (line, "exec", 4) == 0) {
            extract_source (line, current_scripts);
            continue;
        }
        line[strcspn (line, "\n")] = 0;
        char **new_content =
            realloc (scriptmemory.scripts[current_scripts].content,
                     sizeof (char *) *
                     (scriptmemory.scripts[current_scripts].lines + 1));
        if (new_content == NULL) {
            perror ("Failed to allocate memory for line");
            break;
        }
        scriptmemory.scripts[current_scripts].content = new_content;
        scriptmemory.scripts[current_scripts].
            content[scriptmemory.scripts[current_scripts].lines] =
            strdup (line);
        scriptmemory.scripts[current_scripts].lines++;
        scriptmemory.totalLines++;
        if (scriptmemory.totalLines >= MEM_SIZE) {
            perror ("Script memory full");
            exit (1);
        }
    }
    // printf("batch process index is %d\n", current_scripts);
    PCB *pcb = malloc (sizeof (PCB));
    pcb_init (pcb);
    pcb->process_index = current_scripts;
    insert_front (&readyQueue, pcb);
    scriptmemory.totalScripts++;
    // printf("total script now is %d\n", scriptmemory.totalScripts);

    // printf("total scirpts after : %ld\n", scriptmemory.totalScripts);
}
