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
        int *resourceLine = parseResourceLine(line);
        for (int j = 0; j < totalResources; j++) {
            maxResources[i][j] = resourceLine[j];
        }
        free(resourceLine);
        read = getline(&line, &len, file);
    }

    free(line);
}


void calculateTotalThreads(FILE *file) {
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, file)) != -1) {
        totalThreads++;
    }

    rewind(file);
    free(line);
}

bool isSafeState() {
    int work[MAX_RESOURCES];
    bool finish[MAX_THREADS];
    bool safe = true;

    memcpy(work, availableResources, sizeof(int) * totalResources);
    memset(finish, false, sizeof(bool) * totalThreads);

    for (int i = 0; i < totalThreads; i++) {
        for (int j = 0; j < totalThreads; j++) {
            if (!finish[j]) {
                int k;
                for (k = 0; k < totalResources; k++) {
                    if (maxResources[j][k] - allocatedResources[j][k] > work[k]) {
                        break;
                    }
                }

                if (k == totalResources) {
                    for (k = 0; k < totalResources; k++) {
                        work[k] += allocatedResources[j][k];
                    }

                    finish[j] = true;
                }
            }
        }
    }

    for (int i = 0; i < totalThreads; i++) {
        if (!finish[i]) {
            safe = false;
            break;
        }
    }
    return safe;
}

void requestResources(char *buffer) {
    strtok(buffer, " ");
    int threadNumber = atoi(strtok(NULL, " "));
    int customerResources[MAX_RESOURCES];
    int i = 0;
    bool deny = false;

    while (i < totalResources) {
        customerResources[i] = atoi(strtok(NULL, " "));
        i++;
    }

    for (i = 0; i < totalResources; i++) {
        if (customerResources[i] > maxResources[threadNumber][i]) {
            deny = true;
            break;
        }
    }
    if (!deny) {
        for (i = 0; i < totalResources; i++) {
            if (customerResources[i] > availableResources[i]) {
                deny = true;
                break;
            }
        }
    }
    if (!deny) {
        for (i = 0; i < totalResources; i++) {
            availableResources[i] -= customerResources[i];
            allocatedResources[threadNumber][i] += customerResources[i];
        }
    }
    if (deny || !isSafeState()) {
        printf("State is not safe, and the request is not satisfied\n");
    } else {
        printf("State is safe, and the request is satisfied\n");
    }
}

void releaseResources(char *buffer) {
    strtok(buffer, " ");
    int threadNumber = atoi(strtok(NULL, " "));
    int temp[MAX_RESOURCES];
    int i = 0;

    while (i < totalResources) {
        temp[i] = atoi(strtok(NULL, " "));
        i++;
    }

    printf("To release: ");
    for (int j = 0; j < totalResources; j++) {
        availableResources[j] += temp[j];
        allocatedResources[threadNumber][j] -= temp[j];
        printf("%d ", temp[j]);
    }

    printf("\n");

    printf("Now Available Resources: ");
    for (int j = 0; j < totalResources; j++) {
        printf("%d ", availableResources[j]);
    }

    printf("\n");

    printf("Resources still held by the thread: ");
    for (int j = 0; j < totalResources; j++) {
        printf("%d ", allocatedResources[threadNumber][j]);
    }

    printf("\n");
}

void displayStatus() {
    printf("Available Resources:\n");
    for (int i = 0; i < totalResources; i++) {
        printf("%d ", availableResources[i]);
    }
    printf("\n");

    printf("Max Resources:\n");
    for (int i = 0; i < totalThreads; i++) {
        for (int j = 0; j < totalResources; j++) {
            printf("%d ", maxResources[i][j]);
        }
        printf("\n");
    }

    printf("Allocated Resources:\n");
    for (int i = 0; i < totalThreads; i++) {
        for (int j = 0; j < totalResources; j++) {
            printf("%d ", allocatedResources[i][j]);
        }
        printf("\n");
    }

    printf("Need Resources:\n");
    for (int i = 0; i < totalThreads; i++) {
        for (int j = 0; j < totalResources; j++) {
            printf("%d ", maxResources[i][j] - allocatedResources[i][j]);
        }
        printf("\n");
    }
}

void releaseThreadResources(int threadNumber) {
    printf("--> Customer/Thread %d\n", threadNumber);
    printf("Allocated resources: ");
    for (int i = 0; i < totalResources; i++) {
        printf("%d ", allocatedResources[threadNumber][i]);
    }
    printf("\n");

    printf("Needed: ");
    for (int i = 0; i < totalResources; i++) {
        printf("%d ", maxResources[threadNumber][i] - allocatedResources[threadNumber][i]);
    }
    printf("\n");

    printf("Available: ");
    for (int i = 0; i < totalResources; i++) {
        printf("%d ", availableResources[i]);
    }
    printf("\n");

    printf("Thread has started\n");
    printf("Thread has finished\n");
    printf("Thread is releasing resources\n");
    for (int i = 0; i < totalResources; i++) {
        availableResources[i] += allocatedResources[threadNumber][i];
    }

    printf("New Available: ");
    for (int i = 0; i < totalResources; i++) {
        printf("%d ", availableResources[i]);
    }
    printf("\n");
}

void runBankersAlgorithm() {
    int remainingThreads[MAX_THREADS];
    int remainingThreadCount = totalThreads;

    for (int i = 0; i < totalThreads; i++) {
        remainingThreads[i] = i;
    }

    printf("Safe Sequence: ");

    for (int i = 0; i < totalThreads; i++) {
        printf("%d ", remainingThreads[i]);
    }
    printf("\n");

    int counter = 0;
    while (remainingThreadCount > 0 && counter < totalThreads) {
        for (int i = 0; i < remainingThreadCount; i++) {
            int threadValid = 1;
            int threadNumber = remainingThreads[i];

            for (int j = 0; j < totalResources; j++) {
                if (availableResources[j] < (maxResources[threadNumber][j] - allocatedResources[threadNumber][j])) {
                    threadValid = 0;
                    break;
                }
            }
            if (threadValid == 1) {
                for (; i < remainingThreadCount - 1; i++) {
                    remainingThreads[i] = remainingThreads[i + 1];
                }
                i++;
                remainingThreadCount--;
                releaseThreadResources(threadNumber);
            }
        }
        counter++;
    }
}

void executeCommand(char *commandPrefix, char *buffer) {
    if (strcmp(commandPrefix, "Exit") == 0) {
        exit(0);
    } else if (strcmp(commandPrefix, "Run") == 0) {
        runBankersAlgorithm();
    } else if (strcmp(commandPrefix, "Status") == 0) {
        displayStatus();
    } else if (strcmp(commandPrefix, "RQ") == 0) {
        requestResources(buffer);
    } else if (strcmp(commandPrefix, "RL") == 0) {
        releaseResources(buffer);
    } else {
        printf("Invalid input, use one of RQ, RL, Status, Run, Exit\n");
    }
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

    calculateTotalThreads(file);

    printf("Number of Customers: %d\n", totalThreads);
    printf("Currently Available Resources: ");
    for (int i = 0; i < totalResources; i++) {
        printf("%d ", availableResources[i]);
    }
    printf("\n");
    printf("Maximum resources from file:\n");
    readFile(file);

    while (1) {
        char buffer[MAX_COMMAND_LENGTH];
        printf("Enter Command: ");
        if (fgets(buffer, MAX_COMMAND_LENGTH, stdin)) {
            char *commandBuffer = malloc(sizeof(char) * MAX_COMMAND_LENGTH);
            strcpy(commandBuffer, buffer);
            char *commandPrefix = strtok(buffer, " \n");
            executeCommand(commandPrefix, commandBuffer);
            free(commandBuffer);
        }
    }

    fclose(file);

    return 0;
}
