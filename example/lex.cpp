#include <string>
#include <iostream>
#include <stdio.h>
using namespace std;

char ch,cache;
bool retracted,ended;
int num,cur_line;
string token;

void clear_output()
{
    freopen("example.dyd","w",stdout);
    fclose(stdout);
    freopen("LexAnalyze.err","w",stdout);
    fclose(stdout);
}

struct pair_val
{
    string s;
    int val;
};

void getnbc()
{
    if (retracted)
    {
        ch=cache;
        retracted=false;
    }
    else ch=getchar();
    while (ch==' '||ch=='\n')
    {
        if (ch=='\n')
        {
            printf("%16s 24\n","EOLN");
            cur_line++;
        }
        ch=getchar();
    }
}

void concat()
{
    token+=ch;
}

bool letter()
{
    if (ch>='A'&&ch<='Z') return true;
    if (ch>='a'&&ch<='z') return true;
    return false;
}

bool digit()
{
    if (ch>='0'&&ch<='9') return true;
    return false;
}

void retract()
{
    retracted=true;
    cache=ch;
}

int reserve()
{
    if (token=="begin") return 1;
    if (token=="end") return 2;
    if (token=="integer") return 3;
    if (token=="if") return 4;
    if (token=="then") return 5;
    if (token=="else") return 6;
    if (token=="function") return 7;
    if (token=="read") return 8;
    if (token=="write") return 9;
    return 0;
}

void return_val(pair_val v)
{
    freopen("example.dyd","a+",stdout);
    while (v.s.length()<16) v.s=' '+v.s;
    cout<<v.s<<' ';
    printf("%2d\n",v.val);

    if (!(retracted&&cache!=' '&&cache!='\n'))
    {
        getnbc();
        if (ch==EOF)
        {
            printf("%16s 25\n","EOF");
            ended=true;
        }
        retract();
    }
    fclose(stdout);
}

void error(int type)
{
    freopen("LexAnalyze.err","a+",stdout);
    printf("***LINE%d:  ",cur_line);
    if (type==0) printf("illegal operator\n");
    else printf("unknown character\n");
    fclose(stdout);
}

void LexAnalyze()
{
    pair_val ret;
    token="";
    getnbc();

    switch(ch)
    {
        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
        case 'g':
        case 'h':
        case 'i':
        case 'j':
        case 'k':
        case 'l':
        case 'm':
        case 'n':
        case 'o':
        case 'p':
        case 'q':
        case 'r':
        case 's':
        case 't':
        case 'u':
        case 'v':
        case 'w':
        case 'x':
        case 'y':
        case 'z':
        case 'A':
        case 'B':
        case 'C':
        case 'D':
        case 'E':
        case 'F':
        case 'G':
        case 'H':
        case 'I':
        case 'J':
        case 'K':
        case 'L':
        case 'M':
        case 'N':
        case 'O':
        case 'P':
        case 'Q':
        case 'R':
        case 'S':
        case 'T':
        case 'U':
        case 'V':
        case 'W':
        case 'X':
        case 'Y':
        case 'Z':
            while (letter()||digit())
            {
                concat();
                ch=getchar();
            }
            retract();
            num=reserve();
            if (num!=0)
            {
                ret.s=token;
                ret.val=num;
                return_val(ret);
            }
            else
            {
                ret.s=token;
                ret.val=10;
                return_val(ret);
            }
            break;
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            while (digit())
            {
                concat();
                ch=getchar();
            }
            retract();
            ret.s=token;
            ret.val=11;
            return_val(ret);
            break;
        case '=':
            ret.s="=";
            ret.val=12;
            return_val(ret);
        case '<':
            ch=getchar();
            if (ch=='>')
            {
                ret.s="<>";
                ret.val=13;
                return_val(ret);
            }
            else if (ch=='=')
            {
                ret.s="<=";
                ret.val=14;
                return_val(ret);
            }
            else
            {
                retract();
                ret.s="<";
                ret.val=15;
                return_val(ret);
            }
            break;
        case '>':
            ch=getchar();
            if (ch=='=')
            {
                ret.s=">=";
                ret.val=16;
                return_val(ret);
            }
            else
            {
                retract();
                ret.s=">";
                ret.val=17;
                return_val(ret);
            }
            break;
        case '-':
            ret.s="-";
            ret.val=18;
            return_val(ret);
            break;
        case '*':
            ret.s="*";
            ret.val=19;
            return_val(ret);
            break;
        case ':':
            ch=getchar();
            if (ch=='=')
            {
                ret.s=":=";
                ret.val=20;
                return_val(ret);
            }
            else error(0);
            break;
        case '(':
            ret.s="(";
            ret.val=21;
            return_val(ret);
            break;
        case ')':
            ret.s=")";
            ret.val=22;
            return_val(ret);
            break;
        case ';':
            ret.s=";";
            ret.val=23;
            return_val(ret);
            break;
        default:
            error(1);
    }
}

int main()
{
    freopen("example.pas","r",stdin);
    clear_output();
    retracted=ended=false;
    cur_line=1;
    while (!ended)
    {
        LexAnalyze();
    }
    return 0;
}
