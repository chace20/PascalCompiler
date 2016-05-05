#include <stdio.h>
#include <string.h>
#include <cstdlib>

char program[500];
int p = 0; // for moving program array

char token[16];
int category;

int line = 1; // line num
const char *msg;

const char *keywords[9] = {"begin", "end", "integer", "if", "then", "else", "function", "read", "write"};

bool isDigit(char ch) {
    return ch >= '0' && ch <= '9';
}

bool isLetter(char ch) {
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
}

void lexAnalyze() {
    char ch;
    int n = 0;

    for (n = 0; n < 16; n++) token[n] = '\0';
    category = 0;
    n = 0;
    ch = program[p];
    p++;// p is the next

    while (ch == ' ') {
        ch = program[p++];
    }

    if (isDigit(ch)) {
        // is digit
        while (isDigit(ch)) {
            token[n++] = ch;
            ch = program[p++];
        }
        category = 11;
        p--;
    } else if (isLetter(ch)) {
        while (isDigit(ch) || isLetter(ch)) {
            token[n++] = ch;
            ch = program[p++];
        }
        category = 10; // identify
        p--;
        // is keywords
        for (n = 0; n < 9; n++) {
            if (strcmp(token, keywords[n]) == 0) {
                category = n + 1;// keywords category id
                break;
            }
        }
    } else {
        switch (ch) {
            case '=':
                token[0] = '=';
                category = 12;
                break;
            case '<':
                ch = program[p];
                if (ch == '=') {
                    token[0] = '<';
                    token[1] = '=';
                    category = 14;
                    p++;
                } else if (ch == '>') {
                    token[0] = '<';
                    token[1] = '>';
                    category = 13;
                    p++;
                } else {
                    token[0] = '<';
                    category = 15;
                }
                break;
            case '>':
                ch = program[p];
                if (ch == '=') {
                    token[0] = '>';
                    token[1] = '=';
                    category = 16;
                    p++;
                } else {
                    token[0] = '>';
                    category = 17;
                }
                break;
            case '-':
                token[0] = '-';
                category = 18;
                break;
            case '*':
                token[0] = '*';
                category = 19;
                break;
            case ':':
                ch = program[p];
                if (ch == '=') {
                    token[0] = ':';
                    token[1] = '=';
                    category = 20;
                    p++;
                } else {
                    // handle error
                    category = -1;
                    msg = "After \":\" should be \"=\"";
                }
                break;
            case '(':
                token[0] = '(';
                category = 21;
                break;
            case ')':
                token[0] = ')';
                category = 22;
                break;
            case ';':
                token[0] = ';';
                category = 23;
                break;
            case '\n':
                line++;
                token[0] = 'E';
                token[1] = 'O';
                token[2] = 'L';
                token[3] = 'N';
                category = 24;
                break;
            case '\0':
                break;
            default:
                // handle error
                category = -1;
                msg = "Invalid character";
                break;
        }
    }
}

int main() {
    char temp;
    int i = 0;

    FILE *sourceFile;
    FILE *lexFile;
    FILE *errFile;

    // read
    if ((sourceFile = fopen("H:\\Git\\PascalCompiler\\source.pas", "r")) == NULL) {
        printf("can't open source file");
        exit(0);
    }
    while ((temp = fgetc(sourceFile)) != EOF) {
        program[i] = temp;
        i++;
    }
    fclose(sourceFile);

    // write
    if ((lexFile = fopen("H:\\Git\\PascalCompiler\\source.dyd", "w")) == NULL) {
        printf("can't open lex file");
        exit(0);
    }

    // error
    if ((errFile = fopen("H:\\Git\\PascalCompiler\\source.err", "w")) == NULL) {
        printf("can't open error file");
        exit(0);
    }

    while (p < 500) {
        lexAnalyze();
        // if file end
        if (p == 500) {
            token[0] = 'E';
            token[1] = 'O';
            token[2] = 'F';
            category = 25;
        }
        if (category == -1) {
            fputs("***LINE:", errFile);
            fprintf(errFile, "%d", line);
            fputs("  ", errFile);
            fputs(msg, errFile);
            fputs("\n", errFile);

        }else if (category != 0) {
            // print
            printf("%s,", token);
            printf("%d\n", category);
            // write
            for (i = 0; i < 16 - strlen(token); i++)
                fputc(' ', lexFile);// token align right
            fputs(token, lexFile);
            fputc(' ', lexFile);
            if (category < 10)
                fputc(' ', lexFile);// category align right
            fprintf(lexFile, "%d", category);
            fputc('\n', lexFile);
        }
    }

    fclose(errFile);
    fclose(lexFile);
}