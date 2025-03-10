#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void compress(char* inputfile, char* outputfile){
    printf("Compressing %s to %s\n", inputfile, outputfile);
}

void decompress(char* inputfile, char* outputfile){
    printf("DeCompressing %s to %s\n", inputfile, outputfile);
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
        compress(inputFile, outputFile);
    } else if (option == 2)
    {
        printf("Enter the name of the file to decompress: ");
        scanf("%s", inputFile); 
        printf("Enter the name of the output file: ");
        scanf("%s", outputFile);
        decompress(inputFile, outputFile);
    }else{
        printf("Invalid option\n");

        return 1;
    }
    
    return 0;
}