#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>

// define Input stream
typedef struct {
    char *buffer;
    size_t bufferLength;
    size_t inputLength;
}InputBuffer;

// input method
InputBuffer* newInputBuffer() {
    InputBuffer *inputBuffer = malloc(sizeof(InputBuffer));
    inputBuffer->buffer = NULL;
    inputBuffer->bufferLength = 0;
    inputBuffer->inputLength = 0;
    return inputBuffer;
}

void printFormat() {
    printf("db ->");
}

void readInput(InputBuffer* inputBuffer) {
    size_t bytesRead = getline(&(inputBuffer->buffer), &(inputBuffer->bufferLength), stdin);
    if (bytesRead <= 0) {
        printf("Invalid input\n");
        exit(EXIT_FAILURE);
    }
    // ingore '\n'
    inputBuffer->bufferLength = bytesRead -1;
    inputBuffer->buffer[bytesRead - 1] = 0;
}

int main(int argc, char const *argv[])
{
    InputBuffer *inputBuffer = newInputBuffer();
    printf("Sqlite starts...\n");
    while(1) {
        printFormat();
        readInput(inputBuffer);
        if (strcmp(inputBuffer->buffer, ".q") == 0) {
            exit(EXIT_SUCCESS);
        }
        else {
            printf("%s is unsupported , try again~\n", inputBuffer->buffer);
        }
    }
    return 0;
}
