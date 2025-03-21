#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void compress(char* inputText, char* outputfile){
    printf("Compressing to %s\n", outputfile);
}

void decompress(char* inputText, char* outputfile){
    printf("DeCompressing to %s\n", outputfile);
}

char* readFile(char* filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Could not open file %s\n", filename);
        exit(1);
    }
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);
    char *buffer = (char*)malloc(fileSize + 1);
    if (buffer == NULL) {
        printf("Memory allocation failed\n");
        exit(1);
    }
    fread(buffer, 1, fileSize, file);
    buffer[fileSize] = '\0';
    fclose(file);
    return buffer;
}

int main(){
    int option;
    char inputFile[256], outputFile[256];

    printf("Choose an option: \n");
    printf("1. Compress\n");
    printf("2. Decompress\n");
    scanf("%d", &option);

    if (option == 1){
        printf("Enter the name of the file to compress: ");
        scanf("%s", inputFile);
        printf("Enter the name of the output file: ");
        scanf("%s", outputFile);
        char* inputText = readFile(inputFile);
        compress(inputText, outputFile);
        free(inputText);
    } else if (option == 2)
    {
        printf("Enter the name of the file to decompress: ");
        scanf("%s", inputFile); 
        printf("Enter the name of the output file: ");
        scanf("%s", outputFile);
        char* inputText = readFile(inputFile);
        decompress(inputText, outputFile);
        free(inputText);
    }else{
        printf("Invalid option\n");

        return 1;
    }
    
    return 0;
}