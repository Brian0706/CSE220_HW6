#include "hw6.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h> 

typedef enum {FALSE, TRUE} BOOL; 

int setup(int, char**, char**, char**, int*, int*, BOOL*);

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
    else if(searchString == NULL || result == S_ARGUMENT_MISSING){
        return S_ARGUMENT_MISSING;
    }
    else if(replaceString == NULL || result == R_ARGUMENT_MISSING){
        return R_ARGUMENT_MISSING;
    }
    else if(result == L_ARGUMENT_INVALID){
        return result;
    }
    else if(wildcardEnabled && ((*searchString != '*') ==(*(searchString + strlen(searchString) - 1) != '*'))){
        return WILDCARD_INVALID;
    }
    return 0;
}

int setup(int argc, char **argv, char **searchString, 
    char **replaceString, int *startLine, int *endLine, 
    BOOL *wildcardEnabled){
    int opt; 
    char *endPtr;
    int errorStatus = 0;
    while((opt = getopt(argc, argv, ":ws:r:l")) != -1){
        switch(opt){
            case ':':
                printf("Caught Empty\n");
                switch(optopt){
                    case 's':
                        *searchString = "";
                        errorStatus = S_ARGUMENT_MISSING;
                        break;
                    case 'r':
                        *replaceString = "";
                        errorStatus = (errorStatus == S_ARGUMENT_MISSING)
                                    ? errorStatus
                                    : R_ARGUMENT_MISSING;
                        break;
                    case 'l':
                        errorStatus = (errorStatus == S_ARGUMENT_MISSING ||
                                    errorStatus == R_ARGUMENT_MISSING)
                                    ? errorStatus
                                    : L_ARGUMENT_INVALID;
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
                if(*optarg == '\0' || *optarg =='-'){
                    *searchString = "";
                    errorStatus = S_ARGUMENT_MISSING;
                    break;
                }
                *searchString = optarg;
                break;
            case 'r':
                if(*replaceString != NULL){
                    return DUPLICATE_ARGUMENT;
                }
                if(*optarg == '\0' || *optarg =='-'){
                    *replaceString = "";
                    errorStatus = (errorStatus == S_ARGUMENT_MISSING)
                                ? errorStatus
                                : R_ARGUMENT_MISSING;
                    break;
                }
                *replaceString = optarg;
                break;
            case 'l':
                if(*startLine != -1){
                    return DUPLICATE_ARGUMENT;
                }
                if(optarg == NULL){
                    errorStatus = (errorStatus == S_ARGUMENT_MISSING || errorStatus == R_ARGUMENT_MISSING)?errorStatus:L_ARGUMENT_INVALID;
                    continue;
                }
                char larg[strlen(optarg) + 1];
                strncpy(larg, optarg, strlen(optarg));
                larg[strlen(optarg)] = '\0';
                char * token = strtok(larg, ",");
                if(token == NULL){
                    errorStatus = (errorStatus == S_ARGUMENT_MISSING || errorStatus == R_ARGUMENT_MISSING)?errorStatus:L_ARGUMENT_INVALID;
                    continue;
                }
                *startLine = strtol(token, &endPtr, 10);
                if(endPtr == token || *endPtr != '\0'||*startLine < 0){
                    errorStatus = (errorStatus == S_ARGUMENT_MISSING || errorStatus == R_ARGUMENT_MISSING)?errorStatus:L_ARGUMENT_INVALID;
                    continue;
                }
                token = strtok(NULL, ",");
                if(token == NULL){
                    errorStatus = (errorStatus == S_ARGUMENT_MISSING || errorStatus == R_ARGUMENT_MISSING)?errorStatus:L_ARGUMENT_INVALID;
                    continue;
                }
                *endLine = strtol(token, &endPtr, 10);
                if(endPtr == token || *endPtr != '\0' || *endLine < 1|| *endLine < *startLine){
                    errorStatus = (errorStatus == S_ARGUMENT_MISSING || errorStatus == R_ARGUMENT_MISSING)?errorStatus:L_ARGUMENT_INVALID;
                    continue;
                }
                break;
        }
    }
    return errorStatus;
}
