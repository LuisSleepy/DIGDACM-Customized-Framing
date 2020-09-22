#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define OUTPUT "Output.txt"

// https://www.techiedelight.com/pass-2d-array-function-parameter/

char *storingLongStr(char *str, unsigned int maxLength, unsigned int currSize);
int numOfDigits(unsigned num);
void textToBinary(const char *text, int m, int n, char storage[m][n]);

int main() {
    int maxLength = 128;
    int currSize = 0;

    char *inputFromEDC = malloc(maxLength);
    currSize = maxLength;

    inputFromEDC = storingLongStr(inputFromEDC, maxLength, currSize);
    //printf("%s", inputFromEDC);
    //for (int i = 0; i < strlen(inputFromEDC); i++) {
    //    printf("%c", inputFromEDC[i]);
    //}

    int outputFileSize = strlen(OUTPUT);
    //printf("%i", outputFileSize);
    char outputFileBin[outputFileSize][8];

    // Get the binary conversion of the output filename
    //printf("\n");
    textToBinary("Output.txt", outputFileSize, 8, outputFileBin);

    int totSizeOPHeader = outputFileSize * 8;
    char *outputFileHeader;
    outputFileHeader = (char *)malloc(totSizeOPHeader * sizeof(char));

    int index = 0;
    for (int i = 0; i < outputFileSize; i++) {
        for (int j = 0; j < 8; j++) {
            *(outputFileHeader + index) = outputFileBin[i][j];
            index++;
        }
    }

    //for (int i = 0; i < totSizeOPHeader; i++) {
    //    printf("%c", outputFileHeader[i]);
    //}

    // Succeeding line of codes will be for the framing
    int frameTotSize = 16 + totSizeOPHeader + 405;
    int EDCLen = strlen(inputFromEDC);
    int groups = EDCLen / 405;
    int additionalGroup = EDCLen % 405;

    //printf("%i\n", additionalGroup);
    if (additionalGroup != 0) {
        groups++;
    }

    char frames[groups][frameTotSize];

    for (int k = 0; k < groups; k++) {
        int maxEDC = 405;
        char startOF[8] = "00000001";
        char endOT[8] = "00000100";
        int indexOfFrame = 0;

        if (additionalGroup != 0) {
            if (k == groups - 1) {
                maxEDC = additionalGroup;
                //printf("%i\n", maxEDC);
            }
        }

        // For the start byte
        for (int a = 0; a < 8; a++) {
            frames[k][indexOfFrame] = startOF[a];
            indexOfFrame++;
        }

        // For the name of the output file
        for (int b = 0; b < totSizeOPHeader; b++) {
            frames[k][indexOfFrame] = outputFileHeader[b];
            indexOfFrame++;
        }

        //for (int t = 0; t < 8 + totSizeOPHeader; t++) {
        //    printf("%c", frames[k][t]);
        //}
        printf("\n");

        // For the payload
        char parityBits[48];
        int indexOfParityBits = 0;
        // Separate the parity byte
        //printf("%i", maxEDC - 9);
        for (int c = 0; c < maxEDC; c++) {
            if (((c + 1) % 9 == 0) || c >= maxEDC - 9) {
                parityBits[indexOfParityBits] = inputFromEDC[c];
                indexOfParityBits++;
            } else {
                frames[k][indexOfFrame] = inputFromEDC[c];
                indexOfFrame++;
            }
        }

        // Displaying output until payload
        for (int t = 0; t < indexOfFrame; t++) {
            printf("%c", frames[k][t]);
        }
        //printf("\n");

        while (strlen(parityBits) % 8 == 0) {
            int len = strlen(parityBits);
            parityBits[len] = '0';
            parityBits[len + 1] = '\0';
        }

        int parityLen = strlen(parityBits);

        // For the parity bits
        for (int d = 0; d < parityLen; d++) {
            frames[k][indexOfFrame] = parityBits[0];
            indexOfFrame++;
        }

        // For the trailer
        for (int e = 0; e < 8; e++) {
            frames[k][indexOfFrame] = endOT[e];
            indexOfFrame++;
        }

        //for (int t = 0; t < frameTotSize; t++) {
        //    printf("%c", frames[k][t]);
        //}
        //printf("\n");
    }

    // Displaying the final frame(s)
    int totSizeFrame = groups * frameTotSize;
    char *frameOutput;
    frameOutput = (char *)malloc(totSizeFrame * sizeof(char));

    int frameIdx = 0;
    for (int f = 0; f < groups; f++) {
        for (int g = 0; g < 8; g++) {
            char c = frames[f][g];
            if (c == '\0') {
                break;
            } else {
                *(frameOutput + frameIdx) = frames[f][g];
                //printf("%c", *(frameOutput + frameIdx));
                frameIdx++;
            }
        }
    }

    //for (int h = 0; h < totSizeFrame; h++) {
    //    printf("%c", frameOutput[h]);
    //}

    free(outputFileHeader);
    free(inputFromEDC);
    inputFromEDC= NULL;
    return 0;
}

int numOfDigits(unsigned num) {
    if (num < 10) {
        return 1;
    }
    return 1 + numOfDigits(num / 10);
}

// Dynamic allocation for the inputted data
char *storingLongStr(char *str, unsigned int maxLength, unsigned int currSize) {
    if (str != NULL) {
        int c = EOF;
        unsigned int i = 0;

        // Accept user input until hit enter or end of file
        while ((c = getchar()) != '\n' && c != EOF) {
            str[i++] = (char) c;

            // If i reaches maximum size, realloc activated
            if (i == currSize) {
                currSize = i + maxLength;
                str = realloc(str, currSize);
            }
        }

        str[i] = '\0';
    }
    return str;
}

// Binary conversion
void textToBinary(const char *text, int m, int n, char storage[m][n]) {
    for (int i = 0; i < m; i++) {
        unsigned char t = text[i];
        int number = t;
        char *binaryChar;

        binaryChar = (char *)malloc(32 + 1);

        int counter = 0;
        while (number > 0) {
            *(binaryChar + counter) = (number % 2) + '0';
            //printf("%c", binaryChar[i]);
            number = number / 2;
            counter++;
        }
        //printf("\n");
        strrev(binaryChar);

        int additionalZero = 8 - counter;
        int counterCopy = counter;
        if (additionalZero != 0) {
            for (int x = 0; x < additionalZero; x++) {
                storage[i][x] = '0';
                counter++;
            }
        }

        int idxOfStorage = additionalZero;
        for (int y = 0; y < counterCopy; y++) {
            storage[i][idxOfStorage] = binaryChar[y];
            idxOfStorage++;
        }

        //for (int b = 0; b < 8; b++) {
        //    printf("%c", *((storage + i) + b));
        //}
        //printf("\n");
    }
}


