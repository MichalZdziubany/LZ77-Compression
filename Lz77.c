#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Define buffer sizes for memory performance and efficiency
//Higher sizes lead to slower program but higher compression rate
#define Search_Buffer_Size 4096
#define Look_Ahead_Buffer_Size 16

//Create structure to store the compression metadata as tokens
typedef struct {
    int offset;
    int length;
    char next;
} LZ77Token;

//Function to compress the bytes read from a text file
//Parameters: inputText - the data read in binary format from the file inputted by the user
//          outputfile - the name of the file where the compressed data will be written
void compress(char* inputText, char* outputfile){
    printf("Compressing to %s\n", outputfile);

    int inputLength = strlen(inputText);
    //Allocate memory for tokens, assuming worst case that each character is a token
    LZ77Token* tokens = (LZ77Token*)malloc(inputLength * sizeof(LZ77Token));
    int tokenCount = 0;
    int i = 0;

    //main loop
    while (i < inputLength) {
        int maxLength = 0;
        int maxOffset = 0;

        //Search for the longest match in the search buffer
        for (int j = 1; j < Search_Buffer_Size && j <= i; j++){
            int length = 0;
            //compare characters in the search buffer with the look ahead buffer
            while (length < Look_Ahead_Buffer_Size && i + length < inputLength && inputText[i - j + length] == inputText[i + length]) {
             length++;
            }
            if (length > maxLength){
                maxLength = length;
                maxOffset = j;
            }
            
        }
        
        //create token for the longest match found
        //If no match is found, set the offset and length to 0
        if (maxLength > 0) {
            tokens[tokenCount].offset = maxOffset;
            tokens[tokenCount].length = maxLength;
            tokens[tokenCount].next = inputText[i + maxLength];
            i += maxLength + 1; //move the current position to the next character after the match
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
    printf("Compression complete");
}

//Function to decompress the compressed data
//Parameters: inputText - the compressed data read from the file
//          outputfile - the name of the file where the decompressed data will be written
void decompress(char* inputText, char* outputfile){
    printf("DeCompressing to %s\n", outputfile);

    //printToFile(outputfile, inputText);
}

//Function that opens a file in binary mode and reads its contents into a dynamically allocated buffer
//Parameters: filename - the name of the file to read
//Returns: a pointer to the buffer containing the file contents
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

//Function that rints the compressed tokens to a file in binary format
//Parameters: outputFile - the name of the file to write to
//          tokens - a pointer to the array of tokens
//          tokenCount - the number of tokens in the array 
void printToFile(char* outputFile, LZ77Token* tokens, int tokenCount){
    FILE *file = fopen(outputFile, "wb");
    if (file == NULL) {
        printf("Could not open file %s\n", outputFile);
        exit(1);
    }
    fwrite(tokens, sizeof(LZ77Token), tokenCount, file);
    fclose(file);

    printf("Debugging Output (Tokens):\n");
    for (int i = 0; i < tokenCount; i++) {
        printf("Token %d: Offset = %d, Length = %d, Next = '%c'\n",
               i, tokens[i].offset, tokens[i].length, tokens[i].next);
    }
}

//Main function handles user input and function calls
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