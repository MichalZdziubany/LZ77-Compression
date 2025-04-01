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
void compress(char* filename, char* outputfile){
    printf("Compressing to %s\n", outputfile);

    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        printf("Could not open file %s\n", filename);
        exit(1);
    }
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);
    char *inputText = (char*)malloc(fileSize + 1);
    if (inputText == NULL) {
        printf("Memory allocation failed\n");
        exit(1);
    }
    fread(inputText, 1, fileSize, file);
    inputText[fileSize] = '\0';
    fclose(file);

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
        
            //Check if we are at the end of the input text
            if (i + maxLength < inputLength) {
                tokens[tokenCount].next = inputText[i + maxLength];
            } else {
                tokens[tokenCount].next = '\0';
            }
        
            i += maxLength + 1;
        } else {
            tokens[tokenCount].offset = 0;
            tokens[tokenCount].length = 0;
        
            //Check if we are at the end of the input text
            if (i < inputLength) {
                tokens[tokenCount].next = inputText[i];
            } else {
                tokens[tokenCount].next = '\0';
            }
        
            i++;
        }
        tokenCount++;
    }

    //Open the file in binary write mode
    FILE *fileWrite = fopen(outputfile, "wb");
    if (fileWrite == NULL) {
        printf("Could not open file %s\n", outputfile);
        exit(1);
    }
    fwrite(tokens, sizeof(LZ77Token), tokenCount, fileWrite);
    fclose(fileWrite);

    /*printf("Debugging Output (Tokens):\n");
    for (int i = 0; i < tokenCount; i++) {
        printf("Token %d: Offset = %d, Length = %d, Next = '%s'\n",
            i, tokens[i].offset, tokens[i].length,
            tokens[i].next == '\0' ? "\\0" : (char[]){tokens[i].next, '\0'});
    }
    */
    free(tokens);
    printf("Compression complete\n");
}

//Function to decompress the compressed data
//Parameters: inputText - the compressed data read from the file
//          outputfile - the name of the file where the decompressed data will be written
void decompress(char* inputFile, char* outputfile){
    printf("DeCompressing to %s\n", outputfile);

    // Open the input file in binary read mode
    FILE *file = fopen(inputFile, "rb");
    if (file == NULL) {
        printf("Could not open file %s\n", inputFile);
        exit(1);
    }

    // Get the size of the input file
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Allocate memory for the compressed data
    char *inputText = (char *)malloc(fileSize);
    if (inputText == NULL) {
        printf("Memory allocation failed\n");
        fclose(file);
        exit(1);
    }

    // Read the file contents into the buffer
    fread(inputText, 1, fileSize, file);
    fclose(file);

    // Calculate the number of tokens
    int tokenCount = fileSize / sizeof(LZ77Token);

    // Debugging: Print the number of tokens
    //printf("Number of tokens: %d\n", tokenCount);

    //Allocate memory for the decompressed text with an initial size
    int bufferSize = Search_Buffer_Size * 10;
    char *decompressedText = (char *)malloc(bufferSize);
    if (decompressedText == NULL) {
        printf("Memory allocation failed\n");
        free(inputText);
        exit(1);
    }
    int decompressedLength = 0;

    //Reconstruct the original text using the tokens
    LZ77Token *tokens = (LZ77Token *)inputText;
    for (int i = 0; i < tokenCount; i++) {
        LZ77Token token = tokens[i];

        /*printf("Token %d: Offset = %d, Length = %d, Next = '%s'\n",
            i, token.offset, token.length,
            token.next == '\0' ? "\\0" : (char[]){token.next, '\0'});
        */
        //Validate token values
        if (token.offset > decompressedLength || token.offset < 0) {
            printf("Error: Invalid token offset %d at token %d\n", token.offset, i);
            free(inputText);
            free(decompressedText);
            exit(1);
        }
        if (token.length < 0 || token.length > Look_Ahead_Buffer_Size) {
            printf("Error: Invalid token length %d at token %d\n", token.length, i);
            free(inputText);
            free(decompressedText);
            exit(1);
        }

        //Resize the buffer if needed
        while (decompressedLength + token.length + 1 >= bufferSize) {
            bufferSize *= 2;
            decompressedText = (char *)realloc(decompressedText, bufferSize);
            if (decompressedText == NULL) {
                printf("Memory reallocation failed\n");
                free(inputText);
                exit(1);
            }
        }

        //Copy the matching substring from the decompressed text
        if (token.length > 0) {
            int start = decompressedLength - token.offset;
            for (int j = 0; j < token.length; j++) {
                decompressedText[decompressedLength] = decompressedText[start + j];
                decompressedLength++;
            }
        }

        //Append the next character
        if (token.next != '\0') {
            decompressedText[decompressedLength] = token.next;
            decompressedLength++;
        } /*else {
            printf("Token %d: Next is '\\0', skipping append.\n", i);
        }*/
    }

    //Null-terminate the decompressed text
    decompressedText[decompressedLength] = '\0';

    //printf("decompressedText = %s\n", decompressedText);

    //Write the decompressed text to the output file as plain text
    FILE *fileWrite = fopen(outputfile, "w");
    if (fileWrite == NULL) {
        printf("Could not open file %s for writing\n", outputfile);
        free(decompressedText);
        free(inputText);
        exit(1);
    }
    fprintf(fileWrite, "%s", decompressedText);
    fclose(fileWrite);

    //Free allocated memory
    free(decompressedText);
    free(inputText);

    printf("Decompression complete\n");
}


//Main function handles user input and function calls
int main(){
    int option;
    char inputFile[256], outputFile[256];

    do {
        printf("Choose an option: \n");
        printf("1. Compress\n");
        printf("2. Decompress\n");
        printf("-1. Exit\n");
        scanf("%d", &option);

        if (option == 1) {
            printf("Enter the name of the file to compress (e.g., input.txt): ");
            scanf("%s", inputFile);
            printf("Enter the name of the output compressed file (e.g., compressed.bin): ");
            scanf("%s", outputFile);
            compress(inputFile, outputFile);
        } else if (option == 2) {
            printf("Enter the name of the compressed file to decompress (e.g., compressed.bin): ");
            scanf("%s", inputFile);
            printf("Enter the name of the output decompressed file (e.g., output.txt): ");
            scanf("%s", outputFile);
            decompress(inputFile, outputFile);
        }
    } while (option != -1);
    printf("Exiting program.\n");

    return 0;
}