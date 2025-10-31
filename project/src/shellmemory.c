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
    for (size_t i = 0; i < scriptmemory.capacity; i++) {
        scriptmemory.scripts[i].content = NULL;
        scriptmemory.scripts[i].lines = 0;
    }
}

// Free allocated memory
void script_free () {
    for (size_t i = 0; i < scriptmemory.capacity; i++) {
        Script *script = &scriptmemory.scripts[i];
        if (script->content != NULL) {
            for (size_t j = 0; j < script->lines; j++) {
                free (script->content[j]);
            }
            free (script->content);
        }
        script->content = NULL;
        script->lines = 0;
    }
    scriptmemory.totalScripts = 0;
    scriptmemory.totalLines = 0;
}
void script_free_index (int index) {
    if (index < 0) {
        return;
    }
    size_t idx = (size_t) index;
    if (idx >= scriptmemory.totalScripts) {
        return;
    }

    Script *script = &scriptmemory.scripts[idx];
    if (script->content != NULL) {
        for (size_t j = 0; j < script->lines; j++) {
            free (script->content[j]);
        }
        free (script->content);
    }
    if (scriptmemory.totalLines >= script->lines) {
        scriptmemory.totalLines -= script->lines;
    } else {
        scriptmemory.totalLines = 0;
    }
    script->content = NULL;
    script->lines = 0;
}

int script_add_script (const char *source) {

    int newlines = count_lines (source);
    if (newlines < 0) {
        return 1;
    }
    if (scriptmemory.totalLines + (size_t) newlines >= MEM_SIZE) {
        perror ("Script memory full");
        return 1;
    }
    // resize if necessary
    if (scriptmemory.totalScripts >= scriptmemory.capacity) {
        size_t new_capacity = scriptmemory.capacity * 2;
        Script *new_scripts = realloc (scriptmemory.scripts,
                                       sizeof (Script) * new_capacity);
        if (new_scripts == NULL) {
            perror ("Failed to reallocate");
            return 1;
        }
        for (size_t i = scriptmemory.capacity; i < new_capacity; i++) {
            new_scripts[i].content = NULL;
            new_scripts[i].lines = 0;
        }
        scriptmemory.scripts = new_scripts;
        scriptmemory.capacity = new_capacity;
    }
    size_t slot = scriptmemory.totalScripts;
    script_load_script ((int) slot, source);
    scriptmemory.totalLines += scriptmemory.scripts[slot].lines;
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
    if (script_id < 0) {
        fclose (p);
        return;
    }
    size_t idx = (size_t) script_id;
    if (idx >= scriptmemory.capacity) {
        fclose (p);
        return;
    }
    Script *script = &scriptmemory.scripts[idx];
    if (script->content != NULL) {
        for (size_t j = 0; j < script->lines; j++) {
            free (script->content[j]);
        }
        free (script->content);
    }
    script->content = NULL;
    script->lines = 0;

    char line[MAX_LINE_LENGTH];
    while (fgets (line, sizeof (line), p) != NULL) {
        // Remove newline character if present
        line[strcspn (line, "\n")] = 0;

        // Allocate memory for the new line
        char **new_content = realloc (script->content,
                                      sizeof (char *) *
                                      (script->lines + 1));
        if (new_content == NULL) {
            perror ("Failed to allocate memory for line");
            break;
        }
        script->content = new_content;
        script->content[script->lines] = strdup (line);
        if (script->content[script->lines] == NULL) {
            perror ("Failed to duplicate line");
            break;
        }

        script->lines++;
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
        shellmemory[i].var = NULL;
        shellmemory[i].value = NULL;
    }
}

// Set key value pair
void mem_set_value (char *var_in, char *value_in) {
    if (var_in == NULL || value_in == NULL) {
        return;
    }

    for (int i = 0; i < MEM_SIZE; i++) {
        if (shellmemory[i].var != NULL
            && strcmp (shellmemory[i].var, var_in) == 0) {
            char *copy_value = strdup (value_in);
            if (copy_value == NULL) {
                perror ("Failed to allocate shell memory value");
                return;
            }
            free (shellmemory[i].value);
            shellmemory[i].value = copy_value;
            return;
        }
    }

    for (int i = 0; i < MEM_SIZE; i++) {
        if (shellmemory[i].var == NULL) {
            char *copy_var = strdup (var_in);
            char *copy_value = strdup (value_in);
            if (copy_var == NULL || copy_value == NULL) {
                perror ("Failed to allocate shell memory entry");
                free (copy_var);
                free (copy_value);
                return;
            }
            shellmemory[i].var = copy_var;
            shellmemory[i].value = copy_value;
            return;
        }
    }
}

//get value based on input key
char *mem_get_value (char *var_in) {
    if (var_in == NULL) {
        return NULL;
    }
    for (int i = 0; i < MEM_SIZE; i++) {
        if (shellmemory[i].var != NULL
            && strcmp (shellmemory[i].var, var_in) == 0) {
            return shellmemory[i].value;
        }
    }
    return NULL;
}
