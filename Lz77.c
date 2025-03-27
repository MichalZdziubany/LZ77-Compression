#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define Search_Buffer_Size 4096
#define Look_Ahead_Buffer_Size 16

typedef struct {
    int offset;
    int length;
    char next;
} LZ77Token;

void compress(char* inputText, char* outputfile){
    printf("Compressing to %s\n", outputfile);

    int inputLength = strlen(inputText);
    LZ77Token* tokens = (LZ77Token*)malloc(inputLength * sizeof(LZ77Token));
    int tokenCount = 0;
    int i = 0;

    while (i < inputLength) {
        int maxLength = 0;
        int maxOffset = 0;

        for (int j = 0; j < Search_Buffer_Size && j <= i; j++){
            int length = 0;
            while (length < Look_Ahead_Buffer_Size && i + length < inputLength && inputText[i - j + length] == inputText[i + length]){
                length++;
            }
            if (length > maxLength){
                maxLength = length;
                maxOffset = j;
            }
            
        }
        
        if (maxLength > 0) {
            tokens[tokenCount].offset = maxOffset;
            tokens[tokenCount].length = maxLength;
            tokens[tokenCount].next = inputText[i + maxLength];
            i += maxLength + 1;
        } else {
            tokens[tokenCount].offset = 0;
            tokens[tokenCount].length = 0;
            tokens[tokenCount].next = inputText[i];
            i++;
        }
        tokenCount++;
    }
    

    printToFile(outputfile, tokens, tokenCount);
    free(tokens);
}

void decompress(char* inputText, char* outputfile){
    printf("DeCompressing to %s\n", outputfile);

    //printToFile(outputfile, inputText);
}

char* readFile(char* filename) {
    FILE *file = fopen(filename, "rb");
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

void printToFile(char* outputFile, LZ77Token* tokens, int tokenCount){
    FILE *file = fopen(outputFile, "wb");
    if (file == NULL) {
        printf("Could not open file %s\n", outputFile);
        exit(1);
    }
    fwrite(tokens, sizeof(LZ77Token), tokenCount, file);
    fclose(file);

    
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