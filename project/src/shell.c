#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "shell.h"
#include "interpreter.h"
#include "shellmemory.h"

int parseInput (char ui[]);

// Start of everything
int main (int argc, char *argv[]) {
    printf ("Shell version 1.4 created December 2024\n \n");

    fflush (stdout);

    char prompt = '$';          // Shell prompt
    char userInput[MAX_USER_INPUT];     // user's input stored here
    int errorCode = 0;          // zero means no error, default

    //init user input
    for (int i = 0; i < MAX_USER_INPUT; i++) {
        userInput[i] = '\0';
    }
    //init shell memory
    mem_init ();
    script_init ();
    // Check if input is from a terminal or file
    int is_interactive = isatty (STDIN_FILENO);

    while (1) {
        // Only show prompt in interactive mode
        if (is_interactive) {
            printf ("%c ", prompt);
        }
        // Read input
        if (fgets (userInput, MAX_USER_INPUT - 1, stdin) == NULL) {
            // No more input - exit the shell
            break;
        }
        // printf ("User input is : %s\n", userInput);
        // printf("then executing code : ");
        errorCode = parseInput (userInput);
        // Exit conditions
        if (errorCode == -1)
            exit (99);          // ignore all other errors
        memset (userInput, 0, sizeof (userInput));
    }
    return 0;
}

int wordEnding (char c) {
    // You may want to add ';' to this at some point,
    // or you may want to find a different way to implement chains.
    return c == '\0' || c == '\n' || c == ' ';
}

char **arguments;

int parseInput (char inp[]) {
    char tmp[200], *words[100];
    int ix = 0, w = 0, command_size = 0;
    int wordlen;
    int errorCode = 1;
    char *commands[1000];
    char *token;

    token = strtok (inp, ";");
    while (token != NULL) {
        commands[command_size] = token;
        command_size++;
        token = strtok (NULL, ";");
    }

    for (int i = 0; i < command_size; i++) {
        inp = commands[i];
        w = 0;
        memset (tmp, 0, sizeof (tmp));
        memset (words, 0, sizeof (words));
        ix = 0;
        for (ix = 0; inp[ix] == ' ' && ix < 1000; ix++);        // skip white spaces
        while (inp[ix] != '\n' && inp[ix] != '\0' && ix < 1000) {
            // extract a word
            for (wordlen = 0; !wordEnding (inp[ix]) && ix < 1000;
                 ix++, wordlen++) {
                tmp[wordlen] = inp[ix];
            }
            tmp[wordlen] = '\0';
            words[w] = strdup (tmp);
            if (words[w] == NULL) {
                // Handle memory allocation error
                fprintf (stderr, "Memory allocation error\n");
                return -1;
            }
            w++;
            if (inp[ix] == '\0')
                break;
            ix++;
        }
        words[w] = NULL;        // Null-terminate the words array

        errorCode = interpreter (words, w);

        // Free allocated memory
        for (int j = 0; j < w; j++) {
            free (words[j]);
        }
    }
    return errorCode;
}
