#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Define buffer sizes for memory performance and efficiency
//Higher sizes lead to slower program but higher compression rate
#define Search_Buffer_Size 4096
#define Look_Ahead_Buffer_Size 16

//Create structure to store the compression metadata as tokens
typedef struct {
    int offset; //How far back the match starts
    int length; //Length of the match found 
    char next;  //Next character after the match
} LZ77Token;

//Function to compress the bytes read from a text file
//Parameters: filename - the name of the input file to compress
//            outputfile - the name of the file where the compressed data will be written
void compress(char* filename, char* outputfile) {
    printf("Compressing to %s\n", outputfile);

    //Open the file in binary read mode
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        printf("Could not open file %s\n", filename);
        exit(1);
    }

    //Go to the end of the file and get the file size
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    //Go back to the beginning of the file
    fseek(file, 0, SEEK_SET);

    //Create input buffer according to the file size
    char *inputText = (char*)malloc(fileSize + 1);
    if (inputText == NULL) {
        printf("Memory allocation failed\n");
        exit(1);
    }

    //Read the file contents into the buffer and close the file
    fread(inputText, 1, fileSize, file);
    inputText[fileSize] = '\0'; //Null-terminate the input text
    fclose(file);

    //Get the length of the input text
    int inputLength = strlen(inputText);

    //Allocate memory for tokens, assuming worst case that each character is a token
    LZ77Token* tokens = (LZ77Token*)malloc(inputLength * sizeof(LZ77Token));
    int tokenCount = 0; //Counter for the number of tokens created
    int i = 0; //Index for traversing the input text

    //Main loop to process the input text
    while (i < inputLength) {
        int maxLength = 0; //Maximum length of the match found
        int maxOffset = 0; //Offset of the match found

        //Search for the longest match in the search buffer
        for (int j = 1; j < Search_Buffer_Size && j <= i; j++) {
            int length = 0;
            //Compare characters in the search buffer with the look-ahead buffer
            while (length < Look_Ahead_Buffer_Size && i + length < inputLength &&
                   inputText[i - j + length] == inputText[i + length]) {
                length++;
            }
            //Update the maximum match if a longer match is found
            if (length > maxLength) {
                maxLength = length;
                maxOffset = j;
            }
        }

        //Create token for the longest match found
        if (maxLength > 0) {
            tokens[tokenCount].offset = maxOffset;
            tokens[tokenCount].length = maxLength;

            //Check if we are at the end of the input text
            if (i + maxLength < inputLength) {
                tokens[tokenCount].next = inputText[i + maxLength];
            } else {
                tokens[tokenCount].next = '\0'; //No next character
            }

            i += maxLength + 1; //Move the index past the match
        } else {
            //If no match is found, create a token with offset and length set to 0
            tokens[tokenCount].offset = 0;
            tokens[tokenCount].length = 0;

            //Check if we are at the end of the input text
            if (i < inputLength) {
                tokens[tokenCount].next = inputText[i];
            } else {
                tokens[tokenCount].next = '\0'; //No next character
            }

            i++; //Move to the next character
        }
        tokenCount++; //Increment the token count

        //Progress indicator based on bytes processed
        if (i % 100 == 0 || i == inputLength) {
            printf("\rCompression progress: %.2f%%", (double)i / inputLength * 100);
            fflush(stdout);
        }
    }

    //Open the output file in binary write mode
    FILE *fileWrite = fopen(outputfile, "wb");
    if (fileWrite == NULL) {
        printf("Could not open file %s\n", outputfile);
        exit(1);
    }

    //Write the compressed tokens into the output file and close it
    fwrite(tokens, sizeof(LZ77Token), tokenCount, fileWrite);
    fclose(fileWrite);

    //Free the memory allocated for tokens
    free(tokens);
    printf("\nCompression complete\n");
}

//Function to decompress the compressed data
//Parameters: inputFile - the name of the file containing the compressed data
//            outputfile - the name of the file where the decompressed data will be written
void decompress(char* inputFile, char* outputfile) {
    printf("DeCompressing to %s\n", outputfile);

    //Open the input file in binary read mode
    FILE *file = fopen(inputFile, "rb");
    if (file == NULL) {
        printf("Could not open file %s\n", inputFile);
        exit(1);
    }

    //Get the size of the input file
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    //Allocate memory for the compressed data
    char *inputText = (char *)malloc(fileSize);
    if (inputText == NULL) {
        printf("Memory allocation failed\n");
        fclose(file);
        exit(1);
    }

    //Read the file contents into the buffer and close the file
    fread(inputText, 1, fileSize, file);
    fclose(file);

    //Calculate the number of tokens in the compressed data
    int tokenCount = fileSize / sizeof(LZ77Token);

    //Allocate memory for the decompressed text with an initial size
    int bufferSize = Search_Buffer_Size * 10; //Initial buffer size
    char *decompressedText = (char *)malloc(bufferSize);
    if (decompressedText == NULL) {
        printf("Memory allocation failed\n");
        free(inputText);
        exit(1);
    }
    int decompressedLength = 0; //Length of the decompressed text

    //Reconstruct the original text using the tokens
    LZ77Token *tokens = (LZ77Token *)inputText;
    for (int i = 0; i < tokenCount; i++) {
        LZ77Token token = tokens[i];

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

        //Resize the buffer if needed by 50%
        while (decompressedLength + token.length + 1 >= bufferSize) {
            bufferSize += bufferSize / 2;
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
        }

        //Progress indicator based on tokens processed
        if (i % 100 == 0 || i == tokenCount - 1) {
            printf("\rDecompression progress: %.2f%%", (double)(i + 1) / tokenCount * 100);
            fflush(stdout);
        }
    }

    //Null-terminate the decompressed text
    decompressedText[decompressedLength] = '\0';

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

    printf("\nDecompression complete\n");
}

//Main function handles user input and function calls
//Loops until user enters -1 upon which the program terminates
int main() {
    int option; //User's menu choice
    char inputFile[256], outputFile[256]; //File names for input and output

    do {
        //Display menu options
        printf("Choose an option: \n");
        printf("1. Compress\n");
        printf("2. Decompress\n");
        printf("-1. Exit\n");
        scanf("%d", &option);

        if (option == 1) {
            //Handle compression
            printf("Enter the name of the file to compress (e.g., input.txt): ");
            scanf("%s", inputFile);
            printf("Enter the name of the output compressed file (e.g., compressed.bin): ");
            scanf("%s", outputFile);
            compress(inputFile, outputFile);
        } else if (option == 2) {
            //Handle decompression
            printf("Enter the name of the compressed file to decompress (e.g., compressed.bin): ");
            scanf("%s", inputFile);
            printf("Enter the name of the output decompressed file (e.g., output.txt): ");
            scanf("%s", outputFile);
            decompress(inputFile, outputFile);
        }
    } while (option != -1); //Repeat until the user chooses to exit

    printf("Exiting program.\n");
    return 0; //Exit without errors
}