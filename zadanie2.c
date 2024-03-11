#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <inttypes.h>

char* readFile();
int writeToFile(char* outputFileContent, int length);
char* hash(unsigned char *str);
void printError();
void rewriteFile(char* name, char* key, char* fileContent);
char* str_replace(char* string, const char* substr, const char* replacement);

int main(){
    char* csv = readFile();
    if(csv == NULL) {
        printError();
        return 0;
    }

    char* saveptr1;
    char* saveptr2;
    char* saveptr3;

    char inputName[255];
    printf("meno: ");
    fgets(inputName, 255, stdin);

    char pass[255];
    printf("heslo: ");
    scanf("%s", pass);

    char key[255];
    printf("overovaci kluc: ");
    scanf("%s", key);

    if(strchr(inputName, '\n')){
        inputName[strcspn(inputName, "\n")] = '\0';
    }
    if(strchr(pass, '\n')){
        pass[strcspn(pass, "\n")] = '\0';
    }
    if(strchr(key, '\n')){
        key[strcspn(key, "\n")] = '\0';
    }

    int result = -1;

    char* name;
    char* password;

    char* copiesCsv = calloc(sizeof(char), strlen(csv));
    strcpy(copiesCsv, csv);

    char* lines = strtok_r(copiesCsv, "\n", &saveptr1);
    while (lines != NULL && result == -1) {
        char* copiedLines = calloc(sizeof(char), strlen(lines));
        strcpy(copiedLines, lines);
        char* token = strtok_r(copiedLines, ",", &saveptr2);
        char* copiedToken = calloc(sizeof(char), strlen(token));
        strcpy(copiedToken, token);
        name = strtok_r(copiedToken, ":", &saveptr3);
        if(strcmp(name, inputName) == 0){
            password = strtok_r(NULL, ":", &saveptr3);
            char *hashedPass = hash(pass);
            if(strcmp(hashedPass, password) == 0) {
                char *firstKey = strtok_r(NULL, ":", &saveptr3);
                if(firstKey != NULL && strcmp(key, firstKey) == 0){
                    result = 0;
                    break;
                }
                token = strtok_r(NULL, ",", &saveptr2);
                while (token != NULL && result == -1) {
                    if(token[strlen(token) - 1] == '\r') {
                        token[strlen(token) - 1] = '\0';
                    }
                    if(strcmp(token, key) == 0){
                        result = 0;
                        break;
                    }
                    token = strtok_r(NULL, ",", &saveptr2);
                }
                break;
            } else {
                break;
            }
        }
        lines = strtok_r(NULL, "\n", &saveptr1);
    }

    if(result == 0) {
        rewriteFile(inputName, key, csv);
        printf("ok");
    } else {
        printError();
    }
    return 0;
}

char *strremove(char *str, const char *sub) {
    char *p, *q, *r;
    if (*sub && (q = r = strstr(str, sub)) != NULL) {
        size_t len = strlen(sub);
        while ((r = strstr(p = r + len, sub)) != NULL) {
            memmove(q, p, r - p);
            q += r - p;
        }
        memmove(q, p, strlen(p) + 1);
    }
    return str;
}

void rewriteFile(char* name, char* key, char* fileContent) {
    char *outputString = calloc(sizeof(char), strlen(fileContent));

    char *save1;
    char *save2;
    char *save3;

    char *lines = strtok_r(fileContent, "\n", &save1);
    while (lines != NULL) {
        char* copiedLines = calloc(sizeof(char), strlen(lines));
        strcpy(copiedLines, lines);
        char* nameFromFile = strtok_r(copiedLines, ":", &save2);
        if(strcmp(name, nameFromFile) == 0) {
            char* copiedLine = calloc(sizeof(char), strlen(lines));
            strcpy(copiedLine, lines);
            if(strstr(copiedLine, key)) {
                strremove(copiedLine, key);
            }
            if(strstr(copiedLine, ",,")){
                char* pos = strstr(copiedLine, ",,");
                char* first = malloc(sizeof(char) * (pos - copiedLine) + 1);
                strncpy(first, copiedLine, pos - copiedLine);
                strcat(outputString, first);
                strcat(outputString, pos + 1);
            } else if(strstr(copiedLine, ":,")){
                char* pos = strstr(copiedLine, ":,");
                char* first = malloc(sizeof(char) * (pos - copiedLine) + 1);
                strncpy(first, copiedLine, pos - copiedLine);
                strcat(outputString, first);
                strcat(outputString, ":");
                strcat(outputString, pos + 2);
            }  else if(strstr(copiedLine, ",\r")){
                char *first = strtok(copiedLine, "\r");
                int len = strlen(first);
                first[len] = '\0';
                first[len - 1] = '\r';
                strcat(outputString, first);
            } else if(strstr(copiedLine, ",\0")){
                copiedLine[strlen(copiedLine) - 1] = '\0';
                strcat(outputString, copiedLine);
            } else {
                strcat(outputString, copiedLine);
            }
        } else {
            strcat(outputString, lines);
        }
        lines = strtok_r(NULL, "\n", &save1);
        if(lines != NULL) {
            strcat(outputString, "\n");
        }
    }

    writeToFile(outputString, strlen(outputString));

    return;
}

void printError() {
    printf("chyba");
}

long getFileSize() {
    int file = -1;
    file = open("hesla.csv", O_RDONLY);
    struct stat file_status;
    if (stat("hesla.csv", &file_status) < 0) {
        return -1;
    }
    close(file);
    return file_status.st_size;
}

char* readFile() {
    int file = -1;

    long size = getFileSize("hesla.csv");
    if(size < 0) {
        return NULL;
    }

    file = open("hesla.csv", O_RDONLY);

    if(file == -1){
        return NULL;
    }
    
    char* fileContentToReturn = malloc(size + 1);
    read(file, fileContentToReturn, size);
    close(file);
    return fileContentToReturn;
}

int writeToFile(char* outputFileContent, int length) {
    FILE *fptr;

    fptr = fopen("hesla.csv", "w");
    if(fptr == NULL) {
        printError();
        return -1;
    }

    for (int i = 0; i < length; i++){
        fprintf(fptr, "%c", outputFileContent[i]);
    }

    fclose(fptr);
    return 0;
}

char* hash(unsigned char *str) {
    uint64_t hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c;
    char* hashedStr = malloc(32);
    snprintf(hashedStr, 32, "%"PRIu64, hash);
    return hashedStr;
}
