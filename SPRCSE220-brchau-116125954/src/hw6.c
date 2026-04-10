/*
    Name: Brian Chau
    SBU ID: 116125954
*/

#include "hw6.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h> 

typedef enum {FALSE, TRUE} BOOL; 

int setup(int, char**, char**, char**, int*, int*, BOOL*);

int simpleSearch(const char*, const char*, FILE*, FILE*, int, int);

int wildCardSearch(const char*, const char*, FILE*, FILE*, int, int, BOOL (*) (const char*, const char*));

int replaceAString(const char*, FILE*, char*);

int printToOutput(char*, FILE*);

int testForErrors(FILE* , FILE*);

int argToString(char**, char*, int*);

/*Checks if s2 is a suffix of s1*/
BOOL isSuffix(const char*, const char*);

BOOL isPrefix(const char*, const char*);

int main(int argc, char *argv[]) {
    if(argc < 7){
        return MISSING_ARGUMENT;
    }
    char *inputFile = argv[argc-2];
    char *outputFile = argv[argc-1];
    char *searchString = NULL;
    char *replaceString = NULL;
    int startLine = -1;
    int endLine = -1;
    BOOL wildcardEnabled = FALSE;
    FILE *input;
    FILE *output;
    int result = setup(argc, argv, &searchString, &replaceString,
                        &startLine,&endLine,&wildcardEnabled);
    /*Go through the error code sequentialy ensuring they are
    * returned in the right order.
    */
    if(result == DUPLICATE_ARGUMENT){
        return result;
    }
    else if((input = fopen(inputFile, "r")) == NULL){
        return INPUT_FILE_MISSING;
    }
    else if((output = fopen(outputFile, "w")) == NULL){
        fclose(input);
        return OUTPUT_FILE_UNWRITABLE;
    }
    else if(searchString == NULL || *searchString == '\0'){
        result = S_ARGUMENT_MISSING;
    }
    else if(replaceString == NULL || *replaceString == '\0'){
        result = R_ARGUMENT_MISSING;
    }
    else if(result == L_ARGUMENT_INVALID){
        /*Result is already L_ARGUMENT_INVALID*/
        ;
    }
    else if(wildcardEnabled && ((*searchString != '*') ==(*(searchString + strlen(searchString) - 1) != '*'))){
        result = WILDCARD_INVALID;
    }
    if(result){
        fclose(input);
        fclose(output);
        return result;
    }
    else if(!wildcardEnabled){
        simpleSearch(searchString,replaceString,input,output, startLine, endLine);
    }
    else if(*searchString == '*'){
        BOOL (*searchMode) (const char*, const char*) = isSuffix;
        wildCardSearch((searchString + 1),replaceString,input,output, startLine, endLine, searchMode);
    }
    else{
        size_t length = strlen(searchString);
        char prefix[length];
        strncpy(prefix, searchString, length-1);
        prefix[length-1]='\0';
        BOOL (*searchMode) (const char*, const char*) = isPrefix;
        wildCardSearch(prefix,replaceString,input,output, startLine, endLine, searchMode);
    }
    fclose(input);
    fclose(output);
    return 0;
}

int setup(int argc, char **argv, char **searchString, 
    char **replaceString, int *startLine, int *endLine, 
    BOOL *wildcardEnabled){
    int opt; 
    char *endPtr;
    int errorStatus = 0;
    while((opt = getopt(argc-1, argv, ":ws:r:l:")) != -1){
        switch(opt){
            case ':':
                switch(optopt){
                    case 's':
                        *searchString = "";
                        break;
                    case 'r':
                        *replaceString = "";
                        break;
                    case 'l':
                        errorStatus = L_ARGUMENT_INVALID;
                        break;
                }
                break;
            case 'w':
                if(*wildcardEnabled){
                    return DUPLICATE_ARGUMENT;
                }
                *wildcardEnabled = TRUE;
                break;
            case 's':
                if(argToString(searchString, optarg, &optind)){
                    return DUPLICATE_ARGUMENT;
                }
                else if(optind == argc - 1){
                    *searchString = "";
                }
                break;
            case 'r':
                if(argToString(replaceString, optarg, &optind)){
                    return DUPLICATE_ARGUMENT;
                }
                else if(optind == argc - 1){
                    *replaceString = "";
                }
                break;
            case 'l':
                if(*startLine != -1){
                    return DUPLICATE_ARGUMENT;
                }
                else if(*optarg == '\0'){
                    errorStatus = L_ARGUMENT_INVALID;
                }
                else if(*optarg =='-'){
                    errorStatus = L_ARGUMENT_INVALID;
                    optind--;
                    break;
                }
                else if(optind == argc - 1){
                    errorStatus = L_ARGUMENT_INVALID;
                    break;
                }
                char larg[strlen(optarg) + 1];
                strcpy(larg, optarg);
                char * token = strtok(larg, ",");
                if(token == NULL){
                    errorStatus = L_ARGUMENT_INVALID;
                    break;
                }
                *startLine = strtol(token, &endPtr, 10);
                if(endPtr == token || *endPtr != '\0'||*startLine < 1){
                    errorStatus = L_ARGUMENT_INVALID;
                    break;
                }
                token = strtok(NULL, ",");
                if(token == NULL){
                    errorStatus = L_ARGUMENT_INVALID;
                    break;
                }
                *endLine = strtol(token, &endPtr, 10);
                if(endPtr == token || *endPtr != '\0' || *endLine < 1|| *endLine < *startLine){
                    errorStatus = L_ARGUMENT_INVALID;
                }
                break;
        }
    }
    return errorStatus;
}

int simpleSearch(const char* searchString, const char* replaceString, FILE* input, FILE* output, int start, int end){
    char *testString = (char *) malloc(MAX_SEARCH_LEN + 1);
    *testString='\0';
    int readValue;
    char readChar;
    char* searchTest;
    int lineNumber = 1;
    while((readValue=fgetc(input)) != EOF){
        readChar = (char) readValue;
        /*If the char is a newline, this means that the end of the line has been found*/
        if(readChar == '\n'){
            /*Since word is checked every loop, no need to check just write testString to file*/
            if(strlen(testString) != 0 && printToOutput(testString, output)){
                return FAILED_WRITE;
            }
            /*Write newline to output file*/
            if(fputc('\n', output) == EOF){
                free(testString);
                return FAILED_WRITE;
            }
            /*Increment the line counter*/
            lineNumber++;
            *testString = '\0';
            continue;
        }
        /*Checks if testString is at its max size
        * If so, add the beginning letter and move over all letters by one, then add the new character 
        */
        int stringLength = strlen(testString);
        if(stringLength >= MAX_SEARCH_LEN){
            char charToAdd = *testString;
            if(fputc(charToAdd, output) == EOF){
                free(testString);
                return FAILED_WRITE;
            }
            memmove(testString,testString+1,stringLength--);
        }
        *(testString + stringLength) = readChar;
        *(testString + stringLength + 1) = '\0';
        /*Checks if word is within the range of lines to be replaced*/
        if((lineNumber >= start && lineNumber <= end) || (start == -1 && end == -1)){
            /*Check if searchString is in testString*/
            searchTest = strstr(testString, searchString);
            if(searchTest){
                /*Get all the letters before and after the searchString in testString*/
                size_t beforeSearch = searchTest - testString;
                int remainingChars = strlen(testString) - beforeSearch-(strlen(searchString));
                /*Write the part of testString before the searchString
                * Then write in the replacement string
                */
                if(fwrite(testString, sizeof(char),beforeSearch, output) != beforeSearch){
                    free(testString);
                    return FAILED_WRITE;
                }
                if(replaceAString(replaceString, output, testString)){
                    return FAILED_WRITE;
                }
                /*Just move up everything else rather than writing to file
                * Because remaining chars could be part of another string to replace
                */
                memmove(testString,searchTest + strlen(searchString),remainingChars);
                *(testString + remainingChars) = '\0';
            }
        }
    }
    /*Do one last write to write to output anything left*/
    if(printToOutput(testString, output)){
        return FAILED_WRITE;
    }
    free(testString);
    /*Check for any errors with file pointers*/
    if(testForErrors(input,output)){
        return -1;
    }
    return 0;
}

/*Searchs for and replaces a word that has the suffix or prefix of searchString depending on searchMode*/
int wildCardSearch(const char* searchString, const char* replaceString, FILE* input, FILE* output, int start, int end, BOOL (*searchMode) (const char*, const char*)){
    char *testString = (char *) malloc(MAX_LINE + 1);
    *testString='\0';
    int readValue;
    char readChar;
    int lineNumber = 1;
    while((readValue=fgetc(input)) != EOF){
        readChar = (char) readValue;
        /*If the char is a newline, this means that the end of the line has been found*/
        if(readChar == '\n'){
            /*Check if string should be replaced in output file*/
            if(((lineNumber >= start && lineNumber <= end) || (start == -1 && end == -1)) && searchMode(testString, searchString)){
                /*Write the replacement string to output file, if it fails, exit immediately*/
                if(replaceAString(replaceString, output, testString)){
                    return FAILED_WRITE;
                }
            }
            /*Write the test string to output file, if it fails, exit immediately*/
            else if(strlen(testString) != 0 && printToOutput(testString, output)){
                return FAILED_WRITE;
            }
            /*Adds the newline to the file and then increment the line counter.*/
            if(fputc('\n', output) == EOF){
                free(testString);
                return FAILED_WRITE;
            }
            lineNumber++;
            /*Set the beginning of the array to the null terminator to indicate an empty string*/
            *testString = '\0';
            continue;
        }
        /*Checks if the word has been ended*/
        /*readChar is typecasted as functions are undefined when 
        * charactesr are negative values.
        */
        else if(ispunct((unsigned char) readChar) || isspace((unsigned char) readChar)){
            /*Checks if word is within the range of lines to be replaced*/
            if((lineNumber >= start && lineNumber <= end) || (start == -1 && end == -1)){
                /*Same process as with newline*/
                if(searchMode(testString, searchString)){
                    if(replaceAString(replaceString, output, testString)){
                        return FAILED_WRITE;
                    }
                }
                else{
                    if(printToOutput(testString, output)){
                        return FAILED_WRITE;
                    }
                }
            }
            /*If not in range, just write testString to output*/
            else{
                if(printToOutput(testString, output)){
                    return FAILED_WRITE;
                }
            }
            if(fputc(readChar, output) == EOF){
                free(testString);
                return FAILED_WRITE;
            }
            *testString = '\0';
            continue;
        }
        /*Checks if testString is at its max size
        * If so, add the beginning letter and move over all letters by one, then add the new character 
        */
        int stringLength = strlen(testString);
        if(stringLength >= MAX_LINE - 1){
            char charToAdd = *testString;
            if(fputc(charToAdd, output) == EOF){
                free(testString);
                return FAILED_WRITE;
            }
            memmove(testString,testString+1,stringLength--);
        }
        *(testString + stringLength) = readChar;
        *(testString + stringLength + 1) = '\0';
    }
    /*Do one last check on the testString to check for anything left*/
    if(strlen(testString) != 0){
        if(((lineNumber >= start && lineNumber <= end) || (start == -1 && end == -1)) && searchMode(testString, searchString)){
            if(replaceAString(replaceString, output, testString)){
                return FAILED_WRITE;
            }
        }
        else{
            if(printToOutput(testString, output)){
                return FAILED_WRITE;
            }
        }
    }
    free(testString);
    /*Check for any errors with file pointers*/
    if(testForErrors(input,output)){
        return -1;
    }
    return 0;
}

/*Helper functions are implemented here*/
/*Finds if s2 is a suffix of s1*/
BOOL isSuffix(const char* s1, const char* s2){
    int size1 = strlen(s1);
    int size2 = strlen(s2);
    /*s2 can't be a suffix if it is bigger than s1*/
    if(size1 < size2){
        return FALSE;
    }
    if(strncmp((s1+size1-size2),s2,size2) != 0){
        return FALSE;
    }
    return TRUE;
}

/*Finds if s2 is a prefix of s1*/
BOOL isPrefix(const char* s1, const char* s2){
    int size1 = strlen(s1);
    int size2 = strlen(s2);
    /*s2 can't be a suffix if it is bigger than s1*/
    if(size1 < size2){
        return FALSE;
    }
    if(strncmp(s1,s2,size2) != 0){
        return FALSE;
    }
    return TRUE;
}

/*Writes the replacement string to the file*/
int replaceAString(const char *replaceString, FILE* output, char* testString){
    if(fwrite(replaceString, sizeof(char),strlen(replaceString),output) != strlen(replaceString)){
        /*If write fails, free the testString and then abort*/
        free(testString);
        return FAILED_WRITE;
    }
    return 0;
}

/*Writes the testString to the file*/
int printToOutput(char* testString, FILE* output){
    if(fwrite(testString, sizeof(char),strlen(testString),output) != strlen(testString)){
        /*If write fails, free the testString and then abort*/
        free(testString);
        return FAILED_WRITE;
    }
    return 0;
}

/*Tests the file pointers to see if they have encountered errors*/
int testForErrors(FILE* input , FILE* output){
    if(ferror(input) || ferror(output)){
        return -1;
    }
    return 0;
}

/*Takes the argument provided by optarg and assigns it to dest*/
int argToString(char** dest, char* optarg, int* optind){
    /*Checks if the argument has already been provided*/
    if(*dest != NULL){
        return DUPLICATE_ARGUMENT;
    }
    /*Checks for an argument of empty string*/
    else if(*optarg == '\0'){
        *dest = "";
    }
    /*Checks if the argument is actually just a flag*/
    else if(*optarg =='-'){
        *dest = "";
        /*Ensures that the flag in front is read rather than just skipped over*/
        (*optind)--;
    }
    else{
        /*If all is good, have dest point to the argument*/
        *dest = optarg;
    }
    return 0;
}