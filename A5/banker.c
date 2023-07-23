#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_COMMAND_LENGTH 21
#define MAX_THREADS 100
#define MAX_RESOURCES 10

int totalThreads = 0;
int totalResources = 0;
int maxResources[MAX_THREADS][MAX_RESOURCES];
int allocatedResources[MAX_THREADS][MAX_RESOURCES];
int availableResources[MAX_RESOURCES];

int *parseResourceLine(char *line) {
    int *arr = malloc(sizeof(int) * totalResources);
    int index = 0;
    char *token = strtok(line, ",");
    arr[index++] = atoi(token);
    while ((token = strtok(NULL, ","))) {
        arr[index++] = atoi(token);
    }
    return arr;
}

void readFile(FILE *file) {
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    for (int i = 0; i < totalThreads; i++) {
        maxResources[i] = parseResourceLine(line);
        read = getline(&line, &len, file);
    }

    free(line);
}


int main(int argc, char **argv) {
    FILE* file;
    file = fopen("sample_in_banker.txt", "r");

    if (argc <= 1) {
        exit(0);
    }

    totalResources = argc - 1;
    for (int i = 1; i < argc; i++) {
        availableResources[i - 1] = atoi(argv[i]);
    }
    
    printf("Maximum resources from file:\n");
    readFile(file);

    fclose(file);

    return 0;
}