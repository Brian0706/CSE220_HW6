#include "hw6.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h> 

typedef enum {FALSE, TRUE} BOOL; 

int setup(int, char**, char**, char**, int*, int*, BOOL*);

int simpleSearch(char*, char*, FILE*, FILE*, int, int);

int suffixSearch(char*, char*, FILE*, FILE*, int, int);

int prefixSearch(char*, char*, FILE*, FILE*, int, int);

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
    if(result == DUPLICATE_ARGUMENT){
        return result;
    }
    else if((input = fopen(inputFile, "r")) == NULL){
        return INPUT_FILE_MISSING;
    }
    else if((output = fopen(outputFile, "w")) == NULL){
        return OUTPUT_FILE_UNWRITABLE;
    }
    else if(searchString == NULL || *searchString == '\0'){
        return S_ARGUMENT_MISSING;
    }
    else if(replaceString == NULL || *replaceString == '\0'){
        return R_ARGUMENT_MISSING;
    }
    else if(result == L_ARGUMENT_INVALID){
        return result;
    }
    else if(wildcardEnabled && ((*searchString != '*') ==(*(searchString + strlen(searchString) - 1) != '*'))){
        return WILDCARD_INVALID;
    }
    if(!wildcardEnabled){
        simpleSearch(searchString,replaceString,input,output, startLine, endLine);
    }
    else if(*searchString == '*'){
        suffixSearch((searchString + 1),replaceString,input,output, startLine, endLine);
    }
    else{
        size_t length = strlen(searchString);
        char prefix[length];
        strncpy(prefix, searchString, length-1);
        prefix[length-1]='\0';
        prefixSearch(prefix,replaceString,input,output, startLine, endLine);
    }
    return 0;
}

int setup(int argc, char **argv, char **searchString, 
    char **replaceString, int *startLine, int *endLine, 
    BOOL *wildcardEnabled){
    int opt; 
    char *endPtr;
    int errorStatus = 0;
    while((opt = getopt(argc, argv, ":ws:r:l:")) != -1){
        switch(opt){
            case ':':
                printf("Caught Empty\n");
                switch(optopt){
                    case 's':
                        *searchString = "";
                        break;
                    case 'r':
                        *replaceString = "";
                        break;
                    case 'l':
                        // errorStatus = L_ARGUMENT_INVALID;
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
                if(*searchString != NULL){
                    return DUPLICATE_ARGUMENT;
                }
                else if(*optarg == '\0' || *optarg =='-'){
                    *searchString = "";
                }
                else{
                    *searchString = optarg;
                }
                break;
            case 'r':
                if(*replaceString != NULL){
                    return DUPLICATE_ARGUMENT;
                }
                else if(*optarg == '\0' || *optarg =='-'){
                    *replaceString = "";
                }
                else{
                    *replaceString = optarg;
                }
                break;
            case 'l':
                if(*startLine != -1){
                    return DUPLICATE_ARGUMENT;
                }
                char larg[strlen(optarg) + 1];
                strncpy(larg, optarg, strlen(optarg));
                larg[strlen(optarg)+1] = '\0';
                char * token = strtok(larg, ",");
                if(token == NULL){
                    errorStatus = L_ARGUMENT_INVALID;
                    break;
                }
                *startLine = strtol(token, &endPtr, 10);
                if(endPtr == token || *endPtr != '\0'||*startLine < 0){
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

int simpleSearch(char* searchString, char* replaceString, FILE* input, FILE* output, int start, int end){
    char *testString = (char *) malloc(MAX_SEARCH_LEN + 1);
    char readChar;
    char* searchTest;
    int lineNumber = 1;
    while((readChar=fgetc(input)) != EOF){
        if(readChar == '\n'){
            if(strlen(testString) != 0 && (fwrite(testString, sizeof(char),strlen(testString),output) != strlen(testString))){
                lineNumber++;
                return -3;
            }
            fputc('\n', output);
            lineNumber++;
            *testString = '\0';
            continue;
        }
        int stringLength = strlen(testString);
        if(stringLength >= MAX_SEARCH_LEN - 1){
            char charToAdd = *testString;
            if(fputc(charToAdd, output) == EOF){
                return -3;
            }
            memmove(testString,testString+1,stringLength--);
        }
        *(testString + stringLength) = readChar;
        *(testString + stringLength + 1) = '\0';
        if((lineNumber >= start && lineNumber <= end) || (start == -1 && end == -1)){
            searchTest = strstr(testString, searchString);
            if(searchTest){
                size_t beforeSearch = searchTest - testString;
                int remainingChars = strlen(testString) - beforeSearch-(strlen(searchString));
                if(fwrite(testString, sizeof(char),beforeSearch, output) != beforeSearch){
                    return -3;
                }
                if(fwrite(replaceString, sizeof(char),strlen(replaceString),output) != strlen(replaceString)){
                    return -3;
                }
                memmove(testString,searchTest + strlen(searchString),remainingChars);
                *(testString + remainingChars) = '\0';
            }
        }
    }
    if(strlen(testString) != 0){
        if(fwrite(testString, sizeof(char),strlen(testString),output) != strlen(testString)){
            return -3;
        }
    }
    if(ferror(input)){
        return -1;
    }
    if(ferror(output)){
        return -2;
    }
    free(testString);
    return 0;
}

/*Searchs for and replaces a word that has the suffix of searchString*/
int suffixSearch(char* searchString, char* replaceString, FILE* input, FILE* output, int start, int end){
    char *testString = (char *) malloc(MAX_LINE + 1);
    char readChar;
    int lineNumber = 1;
    while((readChar=fgetc(input)) != EOF){
        if(readChar == '\n'){
            if(strlen(testString) != 0 && (fwrite(testString, sizeof(char),strlen(testString),output) != strlen(testString))){
                lineNumber++;
                return -3;
            }
            fputc('\n', output);
            lineNumber++;
            *testString = '\0';
            continue;
        }
        else if(ispunct(readChar) || isspace(readChar)){
            if((lineNumber >= start && lineNumber <= end) || (start == -1 && end == -1)){
                if(isSuffix(testString, searchString)){
                    if(fwrite(replaceString, sizeof(char),strlen(replaceString),output) != strlen(replaceString)){
                        return -3;
                    }
                }
                else{
                    if(fwrite(testString, sizeof(char),strlen(testString),output) != strlen(testString)){
                        return -3;
                    }
                }
            }
            else{
                if(fwrite(testString, sizeof(char),strlen(testString),output) != strlen(testString)){
                    return -3;
                }
            }
            fputc(readChar, output);
            *testString = '\0';
            continue;
        }
        int stringLength = strlen(testString);
        if(stringLength >= MAX_LINE - 1){
            char charToAdd = *testString;
            if(fputc(charToAdd, output) == EOF){
                return -3;
            }
            memmove(testString,testString+1,stringLength--);
        }
        *(testString + stringLength) = readChar;
        *(testString + stringLength + 1) = '\0';
    }
    if(strlen(testString) != 0){
        if(isSuffix(testString, searchString)){
            if(fwrite(replaceString, sizeof(char),strlen(replaceString),output) != strlen(replaceString)){
                return -3;
            }
        }
        else{
            if(fwrite(testString, sizeof(char),strlen(testString),output) != strlen(testString)){
                return -3;
            }
        }
    }
    if(ferror(input)){
        return -1;
    }
    if(ferror(output)){
        return -2;
    }
    free(testString);
    return 0;
}

/*Searchs for and replaces a word that has the prefix of searchString*/
int prefixSearch(char* searchString, char* replaceString, FILE* input, FILE* output, int start, int end){
    char *testString = (char *) malloc(MAX_LINE + 1);
    char readChar;
    int lineNumber = 1;
    while((readChar=fgetc(input)) != EOF){
        if(readChar == '\n'){
            if(strlen(testString) != 0 && (fwrite(testString, sizeof(char),strlen(testString),output) != strlen(testString))){
                lineNumber++;
                return -3;
            }
            fputc('\n', output);
            lineNumber++;
            *testString = '\0';
            continue;
        }
        else if(ispunct(readChar) || isspace(readChar)){
            if((lineNumber >= start && lineNumber <= end) || (start == -1 && end == -1)){
                if(isPrefix(testString, searchString)){
                    if(fwrite(replaceString, sizeof(char),strlen(replaceString),output) != strlen(replaceString)){
                        return -3;
                    }
                }
                else{
                    if(fwrite(testString, sizeof(char),strlen(testString),output) != strlen(testString)){
                        return -3;
                    }
                }
            }
            else{
                if(fwrite(testString, sizeof(char),strlen(testString),output) != strlen(testString)){
                    return -3;
                }
            }
            fputc(readChar, output);
            *testString = '\0';
            continue;
        }
        int stringLength = strlen(testString);
        if(stringLength >= MAX_LINE - 1){
            char charToAdd = *testString;
            if(fputc(charToAdd, output) == EOF){
                return -3;
            }
            memmove(testString,testString+1,stringLength--);
        }
        *(testString + stringLength) = readChar;
        *(testString + stringLength + 1) = '\0';
    }
    if(strlen(testString) != 0){
        if(isPrefix(testString, searchString)){
            if(fwrite(replaceString, sizeof(char),strlen(replaceString),output) != strlen(replaceString)){
                return -3;
            }
        }
        else{
            if(fwrite(testString, sizeof(char),strlen(testString),output) != strlen(testString)){
                return -3;
            }
        }
    }
    if(ferror(input)){
        return -1;
    }
    if(ferror(output)){
        return -2;
    }
    free(testString);
    return 0;
}

BOOL isSuffix(const char* s1, const char* s2){
    int size1 = strlen(s1);
    int size2 = strlen(s2);
    if(size1 < size2){
        return FALSE;
    }
    if(strncmp((s1+size1-size2),s2,size2) != 0){
        return FALSE;
    }
    return TRUE;
}

BOOL isPrefix(const char* s1, const char* s2){
    int size1 = strlen(s1);
    int size2 = strlen(s2);
    if(size1 < size2){
        return FALSE;
    }
    if(strncmp(s1,s2,size2) != 0){
        return FALSE;
    }
    return TRUE;
}
