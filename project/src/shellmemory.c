#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "shell.h"
#include "shellmemory.h"




//Initialize shell memory
struct memory_struct shellmemory[MEM_SIZE];


/*
  Script memory functions
 */
// Script_memory scriptmemory;
Script_memory scriptmemory;

//init
void script_init () {
    scriptmemory.totalScripts = 0;      // Leave the first index for batch script
    scriptmemory.capacity = 3;
    scriptmemory.totalLines = 0;
    scriptmemory.scripts = malloc (sizeof (Script) * scriptmemory.capacity);
    if (scriptmemory.scripts == NULL) {
        perror ("Failed to allocate memory for scripts");
        exit (1);
    }
}

// Free allocated memory
void script_free () {
    for (int i = 0; i < scriptmemory.totalScripts; i++) {
        for (int j = 0; j < scriptmemory.scripts[i].lines; j++) {
            scriptmemory.scripts[i].content[j] = NULL;
        }
        scriptmemory.scripts[i].lines = 0;
    }
    scriptmemory.totalScripts = 0;

}
void script_free_index (int index) {
    for (int j = 0; j < scriptmemory.scripts[index].lines; j++) {
        scriptmemory.scripts[index].content[j] = NULL;
    }
    scriptmemory.scripts[index].lines = 0;
    scriptmemory.totalScripts--;
}

int script_add_script (const char *source) {

    int newlines = count_lines (source);
    if (scriptmemory.totalLines + newlines >= MEM_SIZE) {
        perror ("Script memory full");
        return 1;
    }
    // resize if necessary
    if (scriptmemory.totalScripts >= scriptmemory.capacity) {
        scriptmemory.capacity *= 2;
        scriptmemory.scripts =
            realloc (scriptmemory.scripts,
                     sizeof (Script) * scriptmemory.capacity);
        if (scriptmemory.scripts == NULL) {
            perror ("Failed to reallocate");
            exit (1);
        }
    }
    script_load_script (scriptmemory.totalScripts, source);
    scriptmemory.totalLines += newlines;
    scriptmemory.totalScripts++;
    return 0;
}

int count_lines (const char *filename) {
    int lines = 0;
    if (filename == NULL) {
        return -1;
    }
    FILE *p = fopen (filename, "rt");
    if (p == NULL) {
        printf ("Failed to open script file: %s\n", filename);

        return -1;
    }
    char line[MAX_LINE_LENGTH];
    while (fgets (line, sizeof (line), p) != NULL) {
        lines++;
    }
    fclose (p);
    return lines;
}
void script_load_script (int script_id, const char *source) {
    FILE *p = fopen (source, "rt");
    if (p == NULL) {
        perror ("Failed to open script file2");
        return;
    }
    char line[MAX_LINE_LENGTH];
    scriptmemory.scripts[script_id].lines = 0;
    scriptmemory.scripts[script_id].content = NULL;

    while (fgets (line, sizeof (line), p) != NULL) {
        // Remove newline character if present
        line[strcspn (line, "\n")] = 0;

        // Allocate memory for the new line
        char **new_content = realloc (scriptmemory.scripts[script_id].content,
                                      sizeof (char *) *
                                      (scriptmemory.scripts[script_id].lines +
                                       1));
        if (new_content == NULL) {
            perror ("Failed to allocate memory for line");
            break;
        }
        scriptmemory.scripts[script_id].content = new_content;
        scriptmemory.scripts[script_id].
            content[scriptmemory.scripts[script_id].lines] = strdup (line);
        if (scriptmemory.
            scripts[script_id].content[scriptmemory.scripts[script_id].
                                       lines] == NULL) {
            perror ("Failed to duplicate line");
            break;
        }

        scriptmemory.scripts[script_id].lines++;
    }

    fclose (p);
}

char **get_script_content (int index) {
    return scriptmemory.scripts[index].content;
}

//Debug function
void print_script () {
    // printf(" there are %d scripts\n", scriptmemory.totalScripts);
    for (int i = 0; i < scriptmemory.totalScripts; i++) {
        printf ("Script %d:\n", i);
        for (int j = 0; j < scriptmemory.scripts[i].lines; j++) {
            printf (" Line: %d ", j);
            printf ("%s", scriptmemory.scripts[i].content[j]);
            printf ("\n");
        }
    }
}



// Helper functions
int match (char *model, char *var) {
    int i, len = strlen (var), matchCount = 0;
    for (i = 0; i < len; i++) {
        if (model[i] == var[i])
            matchCount++;
    }
    if (matchCount == len) {
        return 1;
    } else
        return 0;
}

// Shell memory functions

void mem_init () {
    int i;
    for (i = 0; i < MEM_SIZE; i++) {
        shellmemory[i].var = "none";
        shellmemory[i].value = "none";
    }
}

// Set key value pair
void mem_set_value (char *var_in, char *value_in) {
    int i;

    for (i = 0; i < MEM_SIZE; i++) {
        if (strcmp (shellmemory[i].var, var_in) == 0) {
            shellmemory[i].value = strdup (value_in);
            return;
        }
    }

    //Value does not exist, need to find a free spot.
    for (i = 0; i < MEM_SIZE; i++) {
        if (strcmp (shellmemory[i].var, "none") == 0) {
            shellmemory[i].var = strdup (var_in);
            shellmemory[i].value = strdup (value_in);
            return;
        }
    }
    return;
}

//get value based on input key
char *mem_get_value (char *var_in) {
    int i;
    for (i = 0; i < MEM_SIZE; i++) {
        if (strcmp (shellmemory[i].var, var_in) == 0) {
            return strdup (shellmemory[i].value);
        }
    }
    return NULL;
}
