#include <vector>
#include <string>
#include <iostream>
#include <stdio.h>

using namespace std;

struct variable {
    string vname, vproc, vtype;
    int vkind, vlev, vadr;
} var;

struct procedure {
    string pname, ptype;
    int plev, fadr, ladr;
} proc;

vector<variable> v_table;
vector<procedure> p_table;

string sym;
int val, cur_line, cur_level;
bool ended = false;

void clean_output() {
    freopen("example.dys", "w", stdout);
    fclose(stdout);
    freopen("example.var", "w", stdout);
    fclose(stdout);
    freopen("example.pro", "w", stdout);
    fclose(stdout);
    freopen("SynAnalyze.err", "w", stdout);
    fclose(stdout);
}

void advance() {
    freopen("example.dys", "a+", stdout);
    cin >> sym >> val;
    printf("%16s %2d\n", sym.c_str(), val);
    if (sym == "EOF") ended = true;
    while (sym == "EOLN") {
        cur_line++;
        cin >> sym >> val;
        printf("%16s %2d\n", sym.c_str(), val);
    }
}

void error(int type) {
    freopen("SynAnalyze.err", "a+", stdout);
    switch (type) {
        case 0:
            printf("LINE %d: variable \"%s\" previously declared.\n", cur_line, sym.c_str());
            break;
        case 1:
            printf("LINE %d: missing declaration here.\n",
                   cur_line);
            break;
        case 2:
            printf("LINE %d: missing executive statement here.\n", cur_line);
            break;
        case 3:
            printf("LINE %d: expected \"end\" here.\n",
                   cur_line);
            break;
        case 4:
            printf("LINE %d: expected \"begin\" here.\n",
                   cur_line);
            break;
        case 5:
            printf("LINE %d: missing \';\' here.\n",
                   cur_line);
            break;
        case 6:
            printf("LINE %d: brackets doesn't matched.\n",
                   cur_line);
            break;
        case 7:
            printf("LINE %d: illegal variable here.\n",
                   cur_line);
            break;
        case 8:
            printf("LINE %d: missing \'(\' here.\n",
                   cur_line);
            break;
        case 9:
            printf("LINE %d: illegal declaration here.\n",
                   cur_line);
            break;
        case 10:
            printf("LINE %d: illegal executive statement here.\n",cur_line);break;
        case 11:
            printf("LINE %d: variable \"%s\" undeclared.\n",cur_line,sym.c_str());break;
        case 12:
            printf("LINE %d: missing \"else\" here.\n",
                   cur_line);
            break;
        case 13:
            printf("LINE %d: missing \"then\" here.\n",
                   cur_line);
            break;
        case 14:
            printf("LINE %d: illegal operator here.\n",
                   cur_line);
            break;
        default:
            printf("LINE %d: unknown error\n", cur_line);
    }
    advance();
    fclose(stdout);
}

void init() {
    clean_output();
    val, cur_line = 1;
    cur_level = 0;
    v_table.clear();
    p_table.clear();
    proc.pname = "";
    proc.ptype = "";
    proc.plev = 0;
    proc.fadr = 0;
    proc.ladr = -1;
    p_table.push_back(proc);
    return;
}

void v_add(int vkind) {
    for (int i = p_table[cur_level].fadr; i <= p_table[cur_level].ladr; i++) {
        if (v_table[i].vname == sym && v_table[i].vlev == cur_level && v_table[i].vkind == vkind) {
            error(0);
            return;
        }
    }
    var.vname = sym;
    var.vproc = p_table[cur_level].pname;
    var.vtype = "integer";
    var.vkind = vkind;
    var.vlev = cur_level;
    var.vadr = v_table.size();
    v_table.push_back(var);
    p_table[cur_level].ladr++;
}

bool v_check() {
    for (int i = p_table[cur_level].fadr; i <= p_table[cur_level].ladr; i++) {
        if (v_table[i].vname == sym && v_table[i].vlev == cur_level) {
            return true;
        }
    }
    return false;
}

void A();

void B();

void C();

void D();

void E();

void F();

void G();

void H();

void I();

void J();

void A() {
    if (sym == "begin") {
        advance();
        if (sym == "integer") {
            advance();
            B();
        }
        else error(1);
        if (sym == ";") {
            while (sym == ";") {
                advance();
                if (sym == "integer") {
                    advance();
                    B();
                }
                else {
                    break;
                }
            }
        }
        else error(2);
        C();
        while (sym == ";") {
            advance();
            C();
        }
        if (sym == "end") {
            advance();
            return;
        }
        else error(3);
    }
    else error(4);
}

void B() {
    if (val == 10) {
        v_add(0);//加入变量
        advance();
    }
    else if (sym == "function") {
        advance();
        if (val == 10) {
            advance();
            proc.pname = sym;
            cur_level++;
            proc.ptype = "integer";
            proc.plev = cur_level;
            proc.fadr = v_table.size();
            proc.ladr = v_table.size() - 1;
            p_table.push_back(proc);
            if (sym == "(") {
                advance();
                if (val == 10) {
                    v_add(1);//加入形参
                    advance();
                    if (sym == ")") {
                        advance();
                        if (sym == ";") {
                            advance();
                            A();
                            cur_level--;
                        }
                        else error(5);
                    }
                    else error(6);
                }
                else error(7);
            }
            else error(8);
        }
        else error(7);
    }
    else error(9);
}

void C() {
    if (sym == "read") {
        advance();
        D();
    }
    else if (sym == "write") {
        advance();
        D();
    }
    else if (val == 10) {
        advance();
        E();
    }
    else if (sym == "if") {
        advance();
        I();
    }
    else error(10);
}

void D() {
    if (sym == "(") {
        advance();
        if (val == 10) {
            if (v_check() == true)//检查变量是否存在
            {
                advance();
                if (sym == ")") {
                    advance();
                }
                else error(6);
            }
            else error(11);
        }
        else error(7);
    }
    else error(8);
}

void E() {
    if (sym == ":=") {
        advance();
        F();
    }
    else error(14);
}

void F() {
    G();
    while (sym == "-") {
        advance();
        F();
    }
}

void G() {
    H();
    while (sym == "*") {
        advance();
        H();
    }
}

void H() {
    if (sym == "F") {
        advance();
        if (sym == "(") {
            advance();
            F();
            if (sym == ")") {
                advance();
            }
            else error(6);
        }
        else error(8);
    }
    else if (val == 10) {
        if (v_check() == true)//检查变量是否存在
        {
            advance();
        }
        else error(11);
    }
    else if (val == 11) {
        advance();
    }
    else error(10);
}

void I() {
    J();
    if (sym == "then") {
        advance();
        C();
        if (sym == "else") {
            advance();
            C();
        }
        else error(12);
    }
    else error(13);
}

void J() {
    F();
    if (val >= 12 && val <= 17) {
        advance();
        F();
    }
    else error(14);
}

int main() {
    freopen("example.dyd", "r", stdin);
    init();
    advance();
    A();
    return 0;
}
