#include <stdio.h>
#include <string.h>
#include <Windows.h>
#define MAX_COUNT 1024
#define SIGN_UNDEFINED_ERR 1
#define SIGN_REDEFINED_ERR 2
#define SIGN_EXECUTE_ERR 3
#define NO_SIGN_ERR 4
#define SIGN_RESERVE_ERR 5
#define NO_PARA_ERR 6
/*types是支持类型的集合*/
typedef enum{ integer } types;
/*记录变量信息的结构体*/
typedef struct {
	char vname[17];
	char vproc[17];
	bool vkind;
	types vtype;
	int vlev;
	int vadr;
} varRecord;
/*记录过程信息的结构体*/
typedef struct{
	char pname[17];
	types ptype;
	int plev;
	int varNum;
	int fadr;
	int ladr;
	int parameter;
	bool parameterIsDefined;
} proRecord;
/**********文法产生式如下**********
A：程序				A->B
B：分程序			B->begin C;M end
C：说明与句表		C->DC'
C'->;DC'|ε
D：说明语句			D->E|J
E：变量说明			E->integer F
F：变量				F->G
G：标识符			G->HG'
G'->HG'|IG'|ε
H：字母				H->a|...|z|A|...|Z
I：数字				I->0|1|...|9
J：函数说明			J->integer function G(K);L
K：参数				K->F
L：函数体			L->begin C;M end
M：执行语句表		M->NM'
M'->;NM'|ε
N：执行语句			N->O|P|Q|W
O：读语句			O->read(F)
P：写语句			P->write(F)
Q：赋值语句			Q->F:=R
R：算术表达式		R->SR'
R'->-SR'|ε
S：项				S->TS'
S'->*TS'|ε
T：因子				T->F|U|Z
U：常数				U->V
V：无符号整数		V->IV'
V'->IV'|ε
W：条件语句			W->if X then N else N
X：条件表达式		X->RYR
Y：关系运算符		Y-><|<=|>|>=|=|<>
Z：函数调用			Z->G(R)
**********************************/
void A();
void B();
void C();
void C_();
void D();
void E();
void F();
void G();
void J();
void K();
void L();
void M();
void M_();
void N();
void O();
void P();
void Q();
void R();
void R_();
void S();
void S_();
void T();
void U();
void W();
void X();
void Y();
void Z();
/*初始化函数：从输入文件读取数据，建立各个文件，初始化全局变量*/
bool init();
/*结束处理函数，将var和pro数组中的元素输出到相应文件，填充输出文件*/
bool final();
/*错误处理函数，参数分别为行号、错误码和错误符号*/
bool error(int lineNum, int errNum, const char* sign);
/*获得所处目录路径，包括最后斜杠，或者为空*/
void getPath(char* in, char* out);
/*获得文件名，不包括扩展*/
void getFilename(char* in, char* out);
/*获得下一符号,true表示已到队尾，false表示还未到队尾*/
bool nextToken();
/*获得当前符号的下一字符,true表示已到'\0'*/
bool nextChar();
/*判断变量是否已存在*/
bool isVarExisted(char* vname, char* vproc, bool vkind);
/*判断过程是否已存在，参数为过程名*/
bool isProExisted(char* vname);
/*获得下一符号，指针不变*/
int getNextToken();

char input[MAX_COUNT][17];//存放输入文件所有符号的数组
int kind[MAX_COUNT];
int inputCount;//输入符号的数量
int pToken;//指向当前输入符号
int pChar;//指向当前输入符号中的当前字符

varRecord currentVar;//存放当前变量的信息
proRecord currentPro;//存放当前过程的信息

int lineNum;//当前行号

varRecord var[MAX_COUNT];//存放变量名表项数组
proRecord pro[MAX_COUNT];//存放过程名表项数组
int varCount;//变量的数量
int proCount;//过程的数量

FILE* inFile;//输入文件句柄
FILE* outFile;//输出文件句柄
FILE* errFile;//错误文件句柄
FILE* varFile;//变量文件句柄
FILE* proFile;//过程文件句柄

/*主函数*/
int main(int argc, char* argv[])
{
	if (init())
		A();
		final();
	return 0;
}
bool init()
{
		//打开文件句柄
		if ((inFile = fopen("source.dyd", "r")) && (outFile = fopen("source.dys", "w")) && (errFile = fopen("syntax.err", "w")) && (varFile = fopen("source.var", "w")) && (proFile = fopen("source.pro", "w")))
		{
			//初始化单词指针、字符指针、行号、层次
			inputCount = 0;
			pToken = 0;
			pChar = 0;
			lineNum = 1;//当前行号
			//level = 0;//当前层次
			//varCountInPro = 0;
			strcpy(currentPro.pname, "");
			currentPro.plev = 0;
			currentPro.varNum = 0;
			currentPro.parameter = -1;

			varCount = 0;
			proCount = 0;
			//读取输入文件内容，初始化input数组
			while (!feof(inFile))
			{
				char stringOfLine[MAX_COUNT];
				if (fgets(stringOfLine, MAX_COUNT, inFile))
				{
					char lineString[20] = "";
					strncpy(lineString, stringOfLine, 19);
					char* kindString = strrchr(lineString, ' ');
					kind[inputCount] = atoi(kindString+1);

					char string[17] = "";
					strncpy(string, stringOfLine, 16);
					char* lastString = strrchr(string, ' ');
					strcpy(input[inputCount], lastString + 1);

					inputCount++;
				}
			}
			return true;
		}
		else
		{
			fclose(inFile);
			fclose(outFile);
			fclose(errFile);
			fclose(varFile);
			fclose(proFile);
			return false;
		}


	}

bool final()
{
	for (int i = 0; i < varCount; i++)
	{
		int vkind = var[i].vkind ? 1 : 0;
		const char *vtype = (var[i].vtype == integer) ? "integer" : "";
		fprintf(varFile, "%16s %16s %d %s %d %d\n", var[i].vname, var[i].vproc, vkind, vtype, var[i].vlev, var[i].vadr);
	}
	for (int i = 0; i < proCount; i++)
	{
		const char *ptype = (pro[i].ptype == integer) ? "integer" : "";
		fprintf(proFile, "%16s %s %d %d %d\n", pro[i].pname, ptype, pro[i].plev, pro[i].fadr, pro[i].ladr);
	}
	if (fseek(inFile, 0, 0) == 0)
	{
		while (!feof(inFile))
			fputc(fgetc(inFile), outFile);

	}

	bool val;
	val = fclose(inFile);
	val = fclose(outFile);
	val = fclose(errFile);
	val = fclose(varFile);
	val = fclose(proFile);
	return val;
}
bool error(int errNum,const char* symbol)
{
	char* errInfo;
	switch (errNum)
	{
	case SIGN_UNDEFINED_ERR:
		fprintf(errFile, "***LINE:%d  %s符号无定义\n", lineNum, input[pToken]);
		break;
	case SIGN_REDEFINED_ERR:
		fprintf(errFile, "***LINE:%d  %s符号重定义\n", lineNum, input[pToken]);
		break;
	case SIGN_EXECUTE_ERR:
		fprintf(errFile, "***LINE:%d  %s处不能匹配执行语句\n", lineNum, input[pToken]);
		break;
	case NO_SIGN_ERR:
		fprintf(errFile, "***LINE:%d  %s处缺少%s\n", lineNum,input[pToken], symbol);
		break;
	case SIGN_RESERVE_ERR:
		errInfo = "以保留字开头";
		break;
	case NO_PARA_ERR:
		fprintf(errFile, "***LINE:%d  缺少形参%s的声明\n", lineNum, symbol);
		break;
	default:
		errInfo = "未知错误";
	}
	return true;
}
void getPath(char* in, char* out)
{
	char* name;
	name = strrchr(in, '\\');
	if (name != NULL)
		strncpy(out, in, strlen(in) - strlen(name) + 1);
	else
		strcpy(out, "");
}
void getFilename(char* in, char* out)
{
	char* fullName;
	char* extension;
	fullName = strrchr(in, '\\');
	extension = strrchr(in, '.');
	if (fullName != NULL)
		strncpy(out, fullName + 1, strlen(fullName) - 1 - strlen(extension));
	else
		strncpy(out, in, strlen(in) - strlen(extension));
}
bool nextToken()
{
	pToken++;
	pChar = 0;
	if (strcmp(input[pToken], "EOF") == 0)
	{
		return true;
	}
	while (strcmp(input[pToken], "EOLN") == 0)
	{
		pToken++;
		lineNum++;
	}
	return false;
}
bool nextChar()
{
	if (input[pToken][pChar] == '\0')
	{
		//nextToken();
		return true;
	}
	pChar++;
	return false;

}
bool isVarExisted(char* vname, char* vproc, bool vkind)
{
	for (int i = 0; i < varCount; i++)
	{
		if ((strcmp(vname, var[i].vname) == 0) && (strcmp(vproc, var[i].vproc) == 0) && (var[i].vkind == vkind))
			return true;
	}
	for (int i = 0; i < proCount; i++)
	{
		if (strcmp(vname, pro[i].pname) == 0)
			return true;
	}
	return false;
}
bool isProExisted(char* vname)
{
	for (int i = 0; i < varCount; i++)
	{
		if (strcmp(vname, var[i].vname) == 0)
			return true;
	}
	for (int i = 0; i < proCount; i++)
	{
		if (strcmp(vname, pro[i].pname) == 0)
			return true;
	}
	return false;
}
int getNextToken()
{
	int pNextToken = pToken + 1;
	while (strcmp(input[pNextToken], "EOLN") == 0)
	{
		pNextToken++;
	}
	return pNextToken;
}

void A()
{
	B();
}
void B()
{
	if (strcmp(input[pToken], "begin") == 0)
	{
		nextToken();
	}
	else
	{
		error(NO_SIGN_ERR,"begin");
		if (strcmp(input[pToken], "integer")!=0)
		{
			nextToken();
		}
	}
	C();
	if (strcmp(input[pToken], ";") == 0)
	{
		nextToken();
	}
	else
	{
		error(NO_SIGN_ERR, ";");
		if ((strcmp(input[pToken], "integer") != 0) && (strcmp(input[pToken], "read") != 0) && (strcmp(input[pToken], "write") != 0) && (kind[pToken]!=10))
		{
			nextToken();
		}
	}
	M();
	if (strcmp(input[pToken], "end") == 0)
	{
		nextToken();
	}
	else
	{
		error(NO_SIGN_ERR, "end");
	}
}
void C()
{
	D();
	C_();
}
void C_()
{
	
	if (strcmp(input[pToken], ";") == 0 && strcmp(input[getNextToken()], "integer") == 0)
	{
		nextToken();
		D();
		C_();
	}
	else
	{
		if (strcmp(input[pToken], "integer") == 0)
		{
			error(NO_SIGN_ERR, ";");
			D();
			C_();
		}
	}

}
void D()
{
	if (strcmp(input[pToken + 1], "function") == 0)
	{
		J();
	}
	else
	{
		E();
	}
}
void E()
{
	if (strcmp(input[pToken], "integer") == 0)
	{
		nextToken();	
	}
	else
	{
		error(NO_SIGN_ERR, "integer");
		//if (kind[pToken] != 10)
		//{
			nextToken();
		//}
	}
	strcpy(currentVar.vname, input[pToken]);
	strcpy(currentVar.vproc, currentPro.pname);
	if (pToken == currentPro.parameter)
	{
		currentVar.vkind = true;
		currentPro.parameterIsDefined = true;
	}
	else
	{
		currentVar.vkind = false;
	}
	currentVar.vtype = integer;
	currentVar.vlev = currentPro.plev;
	currentVar.vadr = varCount;
	if (isVarExisted(input[pToken], currentPro.pname, currentVar.vkind))//如果存在变量
	{
		error(SIGN_REDEFINED_ERR,NULL);
	}
	else 
	{
		if (currentPro.varNum == 0)//如果当前过程中变量数为0，则当前变量是当前过程的第一个变量
		{
			currentPro.fadr = currentVar.vadr;
		}
		currentPro.ladr = currentVar.vadr;//过程中最后一个变量在变量表中的位置
		currentPro.varNum++;//过程中变量数++
		var[varCount] = currentVar;//当前变量存入var数组
		varCount++;//变量数++
	}
	F();
}
void F()
{
	G();
}
void G()
{
	if (kind[pToken] == 10)
	{
		nextToken();
	}
	/*
	char* keywords[] = { "begin", "end", "integer", "if", "then", "else", "function", "read", "write" };
	int keywordLength[] = { 5, 3, 7, 2, 4, 4, 8, 4, 5 };
	for (int i = 0; i < 9; i++)
	{
		if (strncmp(input[pToken], keywords[i], keywordLength[i]) == 0)
		{
			error(SIGN_RESERVE_ERR);
		}
	}
	H();
	G_();
	nextToken();*/
}


void J()
{
	proRecord proBackup = currentPro;//备份当前过程，在匹配完G过程后恢复
	if (strcmp(input[pToken], "integer") == 0)
	{
		nextToken();
	}
	else
	{
		error(NO_SIGN_ERR, "integer");
		if (strcmp(input[pToken], "function") != 0)
		{
			nextToken();
		}
	}
	if (strcmp(input[pToken], "function") == 0)
	{
		nextToken();
	}
	else
	{
		error(NO_SIGN_ERR, "function");
		if (kind[pToken] != 10)
		{
			nextToken();
		}
	}
	strcpy(currentPro.pname, input[pToken]);
	currentPro.ptype = integer;
	currentPro.plev++;
	currentPro.varNum = 0;
	currentPro.parameterIsDefined = false;
	if (isProExisted(input[pToken]))
	{
		error(SIGN_REDEFINED_ERR,NULL);
	}
	G();
	if (strcmp(input[pToken], "(") == 0)
	{
		nextToken();
	}
	else
	{
		error(NO_SIGN_ERR, "(");
		if (kind[pToken] != 10)
		{
			nextToken();
		}
	}
	/*
	strcpy(currentVar.vname, input[pToken]);
	strcpy(currentVar.vproc, currentPro.pname);
	currentVar.vkind = true;
	currentVar.vtype = integer;
	currentVar.vlev = currentPro.plev;
	currentVar.vadr = varCount;
	*/
	currentPro.parameter = pToken;
	K();

	/*
	if (currentPro.varNum == 0)
	{
		currentPro.fadr = currentVar.vadr;
	}
	currentPro.ladr = currentVar.vadr;
	currentPro.varNum++;
	var[varCount] = currentVar;
	varCount++;
	*/
	if (strcmp(input[pToken], ")") == 0)
	{
		nextToken();
	}
	else
	{
		error(NO_SIGN_ERR, ")");
		if (strcmp(input[pToken], ";") != 0)
		{
			nextToken();
		}
	}
	if (strcmp(input[pToken], ";") == 0)
	{
		nextToken();
	}
	else
	{
		error(NO_SIGN_ERR, ";");
		if (strcmp(input[pToken], "begin") != 0)
		{
			nextToken();
		}
	}
	L();
	currentPro = proBackup;//匹配完G过程后恢复原过程
}
void K()
{
	F();
}
void L()
{
	if (strcmp(input[pToken], "begin") == 0)
	{
		nextToken();
	}
	else
	{
		error(NO_SIGN_ERR, "begin");
		if (strcmp(input[pToken], "integer") != 0)
		{
			nextToken();
		}
	}
	C();
	if (!currentPro.parameterIsDefined)
	{
		error(NO_PARA_ERR, input[currentPro.parameter]);
	}
	pro[proCount] = currentPro;//在这里而不是在J()函数最后把currentPro加入pro数组是因为M中可能会使用当前过程(递归)
	proCount++;
	if (strcmp(input[pToken], ";") == 0)
	{
		nextToken();
	}
	else
	{
		error(NO_SIGN_ERR, ";");
		if ((strcmp(input[pToken], "integer") != 0) && (strcmp(input[pToken], "read") != 0) && (strcmp(input[pToken], "write") != 0) && (kind[pToken] != 10))
		{
			nextToken();
		}
	}
	M();
	if (strcmp(input[pToken], "end") == 0)
	{
		nextToken();
	}
	else
	{
		error(NO_SIGN_ERR, "end");
		if ((strcmp(input[pToken], ";") != 0) && (strcmp(input[pToken], "end") != 0))
		{
			nextToken();
		}
	}


	/*if (strcmp(input[pToken], "begin") == 0)
	{
		nextToken();
		C();
		pro[proCount] = currentPro;//在这里而不是在J()函数最后把currentPro加入pro数组是因为M中可能会使用当前过程(递归)
		proCount++;
		if (strcmp(input[pToken], ";") == 0)
		{
			nextToken();
			M();
			if (strcmp(input[pToken], "end") == 0)
			{
				nextToken();
			}
			else
			{
				error(SIGN_MATCH_ERR);
			}
		}
		else
		{
			error(SIGN_MATCH_ERR);
		}
	}
	else
	{
		error(SIGN_MATCH_ERR);
	}*/
	
}
void M()
{
	N();
	M_();
}
void M_()
{
	if (strcmp(input[pToken], ";") == 0)
	{
		nextToken();
		N();
		M_();
	}
	else
	{
		//if ((strcmp(input[pToken], "read") == 0) || (strcmp(input[pToken], "write") == 0)||(strcmp(input[pToken], "if") == 0)||(kind[pToken] == 10))
		if ((strcmp(input[pToken], "end") != 0) && (strcmp(input[pToken],"EOF") !=0 ))
		{
			error(NO_SIGN_ERR, ";");
			N();
			M_();
		}
	}
}
void N()
{
	if (strcmp(input[pToken], "read") == 0)
	{
		O();
	}
	else if (strcmp(input[pToken], "write") == 0)
	{
		P();
	}
	else if (strcmp(input[pToken], "if") == 0)
	{
		W();
	}
	else if (kind[pToken] == 10)
	{
		Q();
	}
	else
	{
		error(SIGN_EXECUTE_ERR, NULL);
		nextToken();
	}
}
void O()
{
	if (strcmp(input[pToken], "read") == 0)
	{
		nextToken();
	}
	else
	{
		error(NO_SIGN_ERR, "read");
		if (strcmp(input[pToken], "(") != 0)
		{
			nextToken();
		}
	}
	if (strcmp(input[pToken], "(") == 0)
	{
		nextToken();
	}
	else
	{
		error(NO_SIGN_ERR, "(");
		if (kind[pToken] != 10)
		{
			nextToken();
		}
	}
	if (!isVarExisted(input[pToken], currentPro.pname, false) && !isVarExisted(input[pToken], currentPro.pname, true))
	{
		error(SIGN_UNDEFINED_ERR,NULL);
	}
	F();
	if (strcmp(input[pToken], ")") == 0)
	{
		nextToken();
	}
	else
	{
		error(NO_SIGN_ERR, ")");
		if ((strcmp(input[pToken], ";") != 0) && (strcmp(input[pToken], "end") != 0))
		{
			nextToken();
		}
	}
}
void P()
{
	if (strcmp(input[pToken], "write") == 0)
	{
		nextToken();
	}
	else
	{
		error(NO_SIGN_ERR, "write");
		if (strcmp(input[pToken], "(") != 0)
		{
			nextToken();
		}
	}
	if (strcmp(input[pToken], "(") == 0)
	{
		nextToken();
	}
	else
	{
		error(NO_SIGN_ERR, "(");
		if (kind[pToken] != 10)
		{
			nextToken();
		}
	}
	if (!isVarExisted(input[pToken], currentPro.pname, false) && !isVarExisted(input[pToken], currentPro.pname, true))
	{
		error(SIGN_UNDEFINED_ERR,NULL);
	}
	F();
	if (strcmp(input[pToken], ")") == 0)
	{
		nextToken();
	}
	else
	{
		error(NO_SIGN_ERR, ")");
		if ((strcmp(input[pToken], ";") != 0) && (strcmp(input[pToken], "end") != 0))
		{
			nextToken();
		}
	}
}
void Q()
{
	if (!isVarExisted(input[pToken], currentPro.pname, false) && !isVarExisted(input[pToken], currentPro.pname, true) && !isProExisted(input[pToken]))
	{
		error(SIGN_UNDEFINED_ERR,NULL);
	}
	F();
	if (strcmp(input[pToken], ":=") == 0)
	{
		nextToken();
	}
	else
	{
		error(NO_SIGN_ERR, ":=");
		if ((kind[pToken] != 10) && (kind[pToken] != 11))
		{
			nextToken();
		}
	}
	R();
}
void R()
{
	S();
	R_();
}
void R_()
{
	if (strcmp(input[pToken], "-") == 0)
	{
		nextToken();
		S();
		R_();
	}
	else
	{
		if ((kind[pToken] == 10) || (kind[pToken] == 11))
		{
			S();
			R_();
		}
	}
}
void S()
{
	T();
	S_();
}
void S_()
{
	if (strcmp(input[pToken], "*") == 0)
	{
		nextToken();
		T();
		S_();
	}
	else
	{
		if ((kind[pToken] == 10) || (kind[pToken] == 11))
		{
			T();
			S_();
		}
	}
}
void T()
{
	if (input[pToken][pChar] >= '0' && input[pToken][pChar] <= '9')
	{
		U();
	}
	else if (strcmp(input[getNextToken()], "(") == 0)///////////////////////pToken+1
	{
		Z();
	}
	else
	{
		if (!isVarExisted(input[pToken], currentPro.pname, false) && !isVarExisted(input[pToken], currentPro.pname, true))
		{
			error(SIGN_UNDEFINED_ERR,NULL);
		}
		F();
	}
}
void U()
{
	if (kind[pToken] == 11)
	{
		nextToken();
	}
}

void W()
{
	if (strcmp(input[pToken], "if") == 0)
	{
		nextToken();
	}
	else
	{
		error(NO_SIGN_ERR, "if");
		if ((kind[pToken] != 10) && (kind[pToken] != 11))
		{
			nextToken();
		}
	}
	X();
	if (strcmp(input[pToken], "then") == 0)
	{
		nextToken();
	}
	else
	{
		error(NO_SIGN_ERR, "then");
		if ((strcmp(input[pToken], "integer") != 0) && (strcmp(input[pToken], "read") != 0) && (strcmp(input[pToken], "write") != 0) && (kind[pToken] != 10))
		{
			nextToken();
		}
	}
	N();
	if (strcmp(input[pToken], "else") == 0)
	{
		nextToken();
	}
	else
	{
		error(NO_SIGN_ERR,"else");
		if ((strcmp(input[pToken], "integer") != 0) && (strcmp(input[pToken], "read") != 0) && (strcmp(input[pToken], "write") != 0) && (kind[pToken] != 10))
		{
			nextToken();
		}
	}
	N();
}
void X()
{
	R();
	Y();
	R();
}
void Y()
{
	if (strcmp(input[pToken], "<") == 0 || strcmp(input[pToken], "<=") == 0 || strcmp(input[pToken], ">") == 0 || strcmp(input[pToken], ">=") == 0 || strcmp(input[pToken], "=") == 0 || strcmp(input[pToken], "<>") == 0)
	{
		nextToken();
	}
	else
	{
		error(NO_SIGN_ERR, "关系运算符");
		if ((kind[pToken] != 10) && (kind[pToken] != 11))
		{
			nextToken();
		}
	}
}
void Z()
{
	if (!isProExisted(input[pToken]))
	{
		error(SIGN_UNDEFINED_ERR, NULL);
	}
	G();
	if (strcmp(input[pToken], "(") == 0)
	{
		nextToken();	
	}
	else
	{
		error(NO_SIGN_ERR, "(");
		if ((kind[pToken] != 10) && (kind[pToken] != 11))
		{
			nextToken();
		}
	}
	R();
	if (strcmp(input[pToken], ")") == 0)
	{
		nextToken();
	}
	else
	{
		error(NO_SIGN_ERR, ")");
		if ((strcmp(input[pToken], "-") != 0) && (strcmp(input[pToken], "*") != 0) && (strcmp(input[pToken], ";") != 0) && (strcmp(input[pToken], "end")))
		{
			nextToken();
		}
	}
}