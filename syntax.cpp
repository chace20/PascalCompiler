//
// Created by chao on 2016/5/5.
//
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_COUNT 1024
// 符号无定义
#define SYMBOL_UNDEFINED 1
// 符号重定义
#define SYMBOL_REDEFINED 2
// 缺少符号
#define SYMBOL_NONE 3
// 符号不匹配
#define SYMBOL_NO_MATCH 4

// types
typedef enum {
    integer
} types;

// var
typedef struct {
    char vname[16];
    char vproc[16];
    int vkind;
    //0-变量 1-形参
    types vtype;
    int vlev;
    int vadr;
} varTable;

// pro
typedef struct {
    char pname[16];
    types ptype;
    int plev;
    int fadr; // 第一个变量在变量表中的位置
    int ladr; // 最后一个变量在变量表中的位置
} proTable;

void A();

void B();

void C();

void C_();

void E();

void F();

void H();

void I();

void I_();

void J();

void K();

void G();

void L();

void M();

void D();

void D_();

void N();

void O();

void P();

void Q();

void S();

void S_();

void T();

void T_();

void U();

void V();

void X();

void X_();

void R();

void Y();

void Z();

void CALL();


void init();

void output();

void error(int lineNum, int errNum, const char *symbol);

// 移动pToken
bool nextToken();

// 移动pChar
bool nextChar();

bool isVarExisted(char *vname, char *vproc, int vkind);

bool isProExisted(char *name);

// 获得下一pToken，pToken本身不变
int getNextToken();

// 保存词法分析获得的符号
char token[MAX_COUNT][16];
// 保存词法分析获得的类型
int category[MAX_COUNT];
// 词法符号的数量
int tokenCount;
// 指向当前符号
int pToken;
// 指向当前符号的当前字符
int pChar;

int lineNum = 1;

varTable currentVar;
proTable currentPro;

varTable varArray[MAX_COUNT];
proTable proArray[MAX_COUNT];
int varCount;
int proCount;

FILE *dydFile;
FILE *dysFile;
FILE *varFile;
FILE *proFile;
FILE *errFile;

int main() {
    init();
    return 0;
}

void init() {
    if ((dydFile = fopen("source.dyd", "r"))
        && (dysFile = fopen("source.dys", "w"))
        && (errFile = fopen("syntax.err", "w"))
        && (varFile = fopen("source.var", "w"))
        && (proFile = fopen("source.pro", "w"))) {
        char lineString[20];
        while (fgets(lineString, MAX_COUNT, dydFile)) {
            lineString[20] = '\0'; // 去掉\n

            char *categoryString = strrchr(lineString, ' ');// 找出最后出现空格的地方
            category[tokenCount] = atoi(categoryString + 1);

            char tempString[16];
            strncpy(tempString, lineString, 16);// 取前16位
            char *tokenString = strrchr(tempString, ' ');
            strcpy(token[tokenCount], tokenString + 1);

            tokenCount++;
        }
    } else {
        printf("can't open file");
        exit(0);
    }
}

void output() {
    for (int i = 0; i < varCount; ++i) {
        const char *vtype = (varArray[i].vtype == integer) ? "integer" : "";
        fprintf(varFile, "%16s %16s %d %s %d %d\n", varArray[i].vname, varArray[i].vproc, varArray[i].vkind, vtype,
                varArray[i].vlev, varArray[i].vadr);
    }
    for (int j = 0; j < proCount; ++j) {
        const char *ptype = (proArray[j].ptype == integer) ? "integer" : "";
        fprintf(proFile, "%16s %s %d %d %d\n", proArray[j].pname, ptype, proArray[j].plev, proArray[j].fadr,
                proArray[j].ladr);
    }

    /* 复制.dyd内容到.dys */
    fseek(dydFile, 0, 0);// 移动到.dyd的文件开始
    char temp;
    while (temp = fgetc(dydFile)) {
        fputc(temp, dysFile);
    }

    fclose(dydFile);
    fclose(dysFile);
    fclose(errFile);
    fclose(proFile);
    fclose(varFile);
}

void error(int lineNum, int errType, const char *symbol) {
    switch (errType) {
        case SYMBOL_NO_MATCH:
            fprintf(errFile, "***LINE: %d %s symbol no match\n", lineNum, token[pToken]);
            break;
        case SYMBOL_NONE:
            fprintf(errFile, "***LINE:%d after %s lack %s\n", lineNum, token[pToken], symbol);
            break;
        case SYMBOL_REDEFINED:
            fprintf(errFile, "***LINE:%d %s symbol redefined\n", lineNum, token[pToken]);
            break;
        case SYMBOL_UNDEFINED:
            fprintf(errFile, "***LINE:%d %s symbol undefined\n", lineNum, token[pToken]);
            break;
        default:
            fprintf(errFile, "***LINE:%d unknow error\n", lineNum);
            break;
    }
}

bool nextToken() {
    pToken++;
    pChar = 0;
    // 如果是换行
    while (strcmp(token[pToken], "EOLN") == 0) {
        pToken++;
        lineNum++;
    }
    // 如果是文件结尾
    if (strcmp(token[pToken], "EOF") == 0) {
        return true;
    }
    return false;
}

bool nextChar() {
    // 如果已经移动到符号结尾
    if (token[pToken][pChar] == '\0') {
        return true;
    }
    pChar++;
    return false;
}

bool isVarExisted(char *vname, char *vproc, int vkind) {
    for (int i = 0; i < varCount; ++i) {
        if ((strcmp(vname, varArray[i].vname) == 0)
            && (strcmp(vproc, varArray[i].vproc))
            && (vkind == varArray[i].vkind)) {
            return true;
        }
    }
    for (int j = 0; j < proCount; ++j) {
        if (strcmp(vname, proArray[j].pname)) {
            return true;
        }
    }
    return false;
}

bool isProExisted(char *vname) {
    for (int i = 0; i < varCount; ++i) {
        if (strcmp(vname, varArray[i].vname)) {
            return true;
        }
    }
    for (int j = 0; j < proCount; ++j) {
        if (strcmp(vname, proArray[j].pname)) {
            return true;
        }
    }
    return false;
}

int getNextToken() {
    // 不移动pToken
    int pNextToken = pToken + 1;
    while (strcmp(token[pNextToken], "EOLN") == 0) {
        pNextToken++;
    }
    return pNextToken;
}

void A() {
    B();
}

void B() {
    if (strcmp(token[pToken], "begin") == 0) {
        nextToken();
    } else {
        error(lineNum, SYMBOL_NONE, "begin");
    }
    C();
    if (strcmp(token[pToken], ";") == 0) {
        nextToken();
    } else {
        error(lineNum, SYMBOL_NONE, ";");
    }
    D();
    if (strcmp(token[pToken], "end") == 0) {
        nextToken();
    } else {
        error(lineNum, SYMBOL_NONE, "end");
    }
}

void C() {
    E();
    C_();
}

void C_() {
    if (strcmp(token[pToken], ";") == 0 && strcmp(token[getNextToken()], "integer") == 0) {
        nextToken();
        E();
        C_();
    } else {
        if (strcmp(token[pToken], "integer") == 0) {
            error(lineNum, SYMBOL_NONE, ";");
            E();
            C_();
        }
    }
}

void E() {
    // TODO 说明语句预判
}



