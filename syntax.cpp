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
// 缺少参数
#define ERR_NO_PARA 5
// 执行语句错误
#define ERR_EXECUTE 6

// types
typedef enum {
    integer
} types;

// var
typedef struct {
    char vname[16];
    char vproc[16];
    int vkind; //0-变量 1-形参
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
    int para; // 过程的参数
    bool paraIsDefined; // 参数是否定义
    int varCount; // 变量个数
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

void W();


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
int tokenCount = 0;
// 指向当前符号
int pToken = 0;
// 指向当前符号的当前字符
int pChar = 0;

int lineNum = 1;

varTable currentVar;
proTable currentPro;

varTable varArray[MAX_COUNT];
proTable proArray[MAX_COUNT];
int varCount = 0;
int proCount = 0;

FILE *dydFile;
FILE *dysFile;
FILE *varFile;
FILE *proFile;
FILE *errFile;

int main() {
    init();
    A();
    output();
    return 0;
}

void init() {
    if ((dydFile = fopen("source.dyd", "r"))
        && (dysFile = fopen("source.dys", "w"))
        && (errFile = fopen("syntax.err", "w"))
        && (varFile = fopen("source.var", "w"))
        && (proFile = fopen("source.pro", "w"))) {
        char lineString[20] = "";

        strcpy(currentPro.pname, "");
        currentPro.plev = 0;
        currentPro.varCount = 0;
        currentPro.para = -1;

        while (fgets(lineString, MAX_COUNT, dydFile)) {
            lineString[19] = '\0'; // 去掉\n

            char *categoryString = strrchr(lineString, ' ');// 找出最后出现空格的地方
            category[tokenCount] = atoi(categoryString + 1);

            char tempString[17] = ""; // 必须要17
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
    while ((temp = fgetc(dydFile)) != EOF) {
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
            printf("***LINE: %d %s symbol no match\n", lineNum, token[pToken]);
            fprintf(errFile, "***LINE: %d %s symbol no match\n", lineNum, token[pToken]);
            break;
        case SYMBOL_NONE:
            printf("***LINE:%d after %s lack %s\n", lineNum, token[pToken], symbol);
            fprintf(errFile, "***LINE:%d after %s lack %s\n", lineNum, token[pToken], symbol);
            break;
        case SYMBOL_REDEFINED:
            printf("***LINE:%d %s symbol redefined\n", lineNum, token[pToken]);
            fprintf(errFile, "***LINE:%d %s symbol redefined\n", lineNum, token[pToken]);
            break;
        case SYMBOL_UNDEFINED:
            printf("***LINE:%d %s symbol undefined\n", lineNum, token[pToken]);
            fprintf(errFile, "***LINE:%d %s symbol undefined\n", lineNum, token[pToken]);
            break;
        case ERR_NO_PARA:
            printf("**LINE:%d procedure lack para %s\n", lineNum, symbol);
            fprintf(errFile, "***LINE:%d procedure lack para %s\n", lineNum, symbol);
            break;
        case ERR_EXECUTE:
            printf("***LINE:%d after %s no execute sentence\n", lineNum, token[pToken]);
            fprintf(errFile, "***LINE:%d after %s no execute sentence\n", lineNum, token[pToken]);
        default:
            printf("***LINE:%d unknow error\n", lineNum);
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
        if (strcmp(vname, proArray[j].pname) == 0) {
            return true;
        }
    }
    return false;
}

bool isProExisted(char *vname) {
    for (int i = 0; i < varCount; ++i) {
        if (strcmp(vname, varArray[i].vname) == 0) {
            return true;
        }
    }
    for (int j = 0; j < proCount; ++j) {
        if (strcmp(vname, proArray[j].pname) == 0) {
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
    if (strcmp(token[pToken], ";") == 0) {
        nextToken();
        E();
        C_();
    } else {
        // TODO empty
        if (strcmp(token[pToken], "integer") == 0) {
            error(lineNum, SYMBOL_NONE, ";");
            E();
            C_();
        }
    }
}

void E() {
    // 用pToken+1而不是getNextToken的原因是getNextToken会忽略换行
    if (strcmp(token[pToken + 1], "function") == 0) {
        G();
    } else {
        F();
    }
}

void F() {
    if (strcmp(token[pToken], "integer") == 0) {
        nextToken();
    } else {
        error(lineNum, SYMBOL_NONE, "integer");
        nextToken();
    }
    // H()放到最后为了匹配更多的错
    H();
}

void H() {
    /* 处理变量 */
    strcpy(currentVar.vname, token[pToken]);
    strcpy(currentVar.vproc, currentPro.pname);
    if (pToken == currentPro.para) {
        currentVar.vkind = 1; // 形参
        currentPro.paraIsDefined = true;
    } else {
        currentVar.vkind = 0;
    }
    currentVar.vtype = integer;
    currentVar.vlev = currentPro.plev;
    currentVar.vadr = varCount;
    if (isVarExisted(token[pToken], currentPro.pname, currentVar.vkind)) {
        error(lineNum, SYMBOL_REDEFINED, token[pToken]);// 变量重复定义
    } else {
        if (currentPro.varCount == 0) {
            currentPro.fadr = currentVar.vadr; // 过程的第一个变量
        }
        currentPro.ladr = currentVar.vadr; // 过程的最后一个变量
        currentPro.varCount++;
        varArray[varCount] = currentVar;
        varCount++;
    }
    // 转到标识符
    I();
}

void I() {
    // 标识符
    if (category[pToken] == 10) {
        nextToken();
    }
}
/*
 * 字母和数字
void J() {}
void K() {}
*/

/* 函数说明 */
void G() {
    proTable proBackup = currentPro;
    if (strcmp(token[pToken], "integer") == 0) {
        nextToken();
    } else {
        error(lineNum, SYMBOL_NONE, "integer");
    }
    if (strcmp(token[pToken], "function") == 0) {
        nextToken();
    } else {
        error(lineNum, SYMBOL_NONE, "function");
    }
    I();
    /* 处理过程 */
    strcpy(currentPro.pname, token[pToken]);
    currentPro.ptype = integer;
    currentPro.plev++;
    currentPro.varCount = 0;
    currentPro.paraIsDefined = false;

    if (isProExisted(token[pToken])) {
        error(lineNum, SYMBOL_REDEFINED, token[pToken]);
    }

    if (strcmp(token[pToken], "(") == 0) {
        nextToken();
    } else {
        error(lineNum, SYMBOL_NONE, "(");
    }

    currentPro.para = pToken;
    L();

    if (strcmp(token[pToken], ")") == 0) {
        nextToken();
    } else {
        error(lineNum, SYMBOL_NONE, ")");
    }
    if (strcmp(token[pToken], ";") == 0) {
        nextToken();
    } else {
        error(lineNum, SYMBOL_NONE, ";");
    }

    M();
    currentPro = proBackup;
}

void L() {
    H();
}

void M() {
    if (strcmp(token[pToken], "begin") == 0) {
        nextToken();
    } else {
        error(lineNum, SYMBOL_NONE, "begin");
    }
    C();
    // TODO
    if (!currentPro.paraIsDefined) {
        error(lineNum, ERR_NO_PARA, token[currentPro.para]);
    }
    proArray[proCount] = currentPro; // 加入procedure表
    proCount++;

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

void D() {
    N();
    D_();
}

void D_() {
    if (strcmp(token[pToken], ";") == 0) {
        nextToken();
        N();
        D_();
    } else {
        if(strcmp(token[pToken], "end") != 0){
            error(lineNum, SYMBOL_NONE, ";");
            N();
            D_();
        }
    }
    // TODO 空的处理
}

void N() {
    if (strcmp(token[pToken], "read") == 0) {
        O();
    } else if (strcmp(token[pToken], "write") == 0) {
        P();
    } else if (strcmp(token[pToken], "if") == 0) {
        R();
    } else if (category[pToken] == 10) {
        // 标识符
        Q();
    } else {
        error(lineNum, ERR_EXECUTE, token[pToken]);
        nextToken();
    }
}

void O() {
    if (strcmp(token[pToken], "read") == 0) {
        nextToken();
    } else {
        error(lineNum, SYMBOL_NONE, "read");
    }
    if (strcmp(token[pToken], "(") == 0) {
        nextToken();
    } else {
        error(lineNum, SYMBOL_NONE, "(");
    }
    if ((!isVarExisted(token[pToken], currentPro.pname, 0)) && (!isVarExisted(token[pToken], currentPro.pname, 1))) {
        error(lineNum, SYMBOL_UNDEFINED, token[pToken]);
    }
    H();
    if (strcmp(token[pToken], ")") == 0) {
        nextToken();
    } else {
        error(lineNum, SYMBOL_NONE, ")");
    }
}

void P() {
    if (strcmp(token[pToken], "write") == 0) {
        nextToken();
    } else {
        error(lineNum, SYMBOL_NONE, "write");
    }
    if (strcmp(token[pToken], "(") == 0) {
        nextToken();
    } else {
        error(lineNum, SYMBOL_NONE, "(");
    }
    if ((!isVarExisted(token[pToken], currentPro.pname, 0)) && (!isVarExisted(token[pToken], currentPro.pname, 1))) {
        error(lineNum, SYMBOL_UNDEFINED, token[pToken]);
    }
    H();
    if (strcmp(token[pToken], ")") == 0) {
        nextToken();
    } else {
        error(lineNum, SYMBOL_NONE, ")");
    }
}

void Q() {
    if ((!isVarExisted(token[pToken], currentPro.pname, 0)) && (!isVarExisted(token[pToken], currentPro.pname, 1)) && (!isProExisted(token[pToken]))) {
        error(lineNum, SYMBOL_UNDEFINED, token[pToken]);
    }
    H();
    if (strcmp(token[pToken], ":=") == 0) {
        nextToken();
    } else {
        error(lineNum, SYMBOL_NONE, ":=");
    }
    S();
}

void S() {
    T();
    S_();
}

void S_() {
    if (strcmp(token[pToken], "-") == 0) {
        nextToken();
        T();
        S_();
    } else {
        if ((category[pToken] == 10) || (category[pToken] == 11)) {
            T();
            S_();
        }
    }
    // TODO empty
}

void T() {
    U();
    T_();
}

void T_() {
    if (strcmp(token[pToken], "*") == 0) {
        nextToken();
        U();
        T_();
    } else {
        if ((category[pToken] == 10) || (category[pToken] == 11)) {
            U();
            T_();
        }
    }
}

void U() {
    if (category[pToken] == 10) {
        if ((!isVarExisted(token[pToken], currentPro.pname, 0)) && (!isVarExisted(token[pToken], currentPro.pname, 1))) {
            error(lineNum, SYMBOL_UNDEFINED, token[pToken]);
        }
        I(); // 标识符
    } else if (category[pToken] == 11) {
        V(); // 常数
    } else if (strcmp(token[getNextToken()], "(") == 0) {
        W(); // 函数调用
    }
}

void V() {
    // 简化了常数和无符号整数
    nextToken();
}

void R() {
    if (strcmp(token[pToken], "if") == 0) {
        nextToken();
    } else {
        error(lineNum, SYMBOL_NONE, "if");
    }
    Y();
    if (strcmp(token[pToken], "then") == 0) {
        nextToken();
    } else {
        error(lineNum, SYMBOL_NONE, "then");
    }
    N();
    if (strcmp(token[pToken], "else") == 0) {
        nextToken();
    } else {
        error(lineNum, SYMBOL_NONE, "else");
    }
    N();
}

void Y() {
    S();
    Z();
    S();
}

void Z() {
    if (category[pToken] >= 12 && category[pToken] <= 17) {
        // 关系运算符
        nextToken();
    } else {
        error(lineNum, SYMBOL_NONE, "relational operator");
    }
}

void W() {
    if (!isProExisted(token[pToken])) {
        error(lineNum, SYMBOL_UNDEFINED, token[pToken]);
    }
    I();
    if (strcmp(token[pToken], "(") == 0) {
        nextToken();
    } else {
        error(lineNum, SYMBOL_NONE, "(");
    }
    S();
    if (strcmp(token[pToken], ")") == 0) {
        nextToken();
    } else {
        error(lineNum, SYMBOL_NONE, ")");
    }
}
