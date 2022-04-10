/*
@project: wordAnalysis
@IDE:    Visual Studio
@Author: Jiang Shutong
@Date:   Oct 10th
*/
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <cstring>
#include <set>
#include <unordered_map>
using namespace std;
int NumOfLine = 0;     //行数
int NumOfKeyWord = 0;  //关键字数目
int NumOfVarible = 0;  //变量名数目
int NumOfNumber = 0;   //数字常量数目
int NumOfOperator = 0; //运算符数目
int NumOfBound = 0;    //界符数目
int flag = 0;
set<string> keywords{//保留字
                     "if", "else", "while",
                     "return", "for", "break",
                     "continue", "do", "int",
                     "char", "double", "void",
                     "short", "bool", "include",
                     "const", "long", "NULL", "scanf",
                     "cin", "cout", "printf", "sizeof"};
set<char> symbols{
    '>', '<', '=', '!', '|', '&',       //比较运算符
    '+', '-', '*', '/', '%',            //算术运算符
    '[', ']', '(', ')', '{', '}',       //界符
    ',', ':', ';', '"', '\'', ' ', '\n' //其他符号
};
unordered_map<string, int> SymbolTable;

bool isLetter(char ch);                        //判断是否为字母
bool isFigure(char ch);                        //判断是否为数字
void OutputStatistics();                       //输出各符号的统计结果
void wordAnalysis(string s);                   //词法分析
void printPair(string sym, string attrtibute); //以二元式的形式输出
void LetterSituation(string s, int &pos, char &now);          //读到的字符为字母时的处理
void FigureSituation(string s, int &pos, char &now);          //读到的字符为数字时的处理
void SymbolSituation(string s, int &pos, char &now, int &state, int &flag);  //读到的字符为其他字符时的处理
void ProcessOtherSymbols(string s, int &pos, char &now, int &state);         //当字符为其他字符时的处理
void ProcessSingleQuotationMarks(string s, int &pos, char &now, int &state); //当字符为单引号时的处理
void ProcessDoubleQuotationMarks(string s, int &pos, char &now, int &state); //当字符为双引号时的处理
void ProcessBoundarySymbol(string s, int &pos, char &now, int &state);       //当字符为界符时的处理
void ProcessCompareSymbol(string s, int &pos, char &now, int &state);        //当字符为比较运算符时的处理
void ProcessPlusSymbol(string s, int &pos, char &now, int &state);           //当字符为加号时的处理
void ProcessMinusSymbol(string s, int &pos, char &now, int &state);          //当字符为减号时的处理
void ProcessLineSymbol(string s, int &pos, char &now, int &state);           //当字符为|时的处理
void ProcessAndSymbol(string s, int &pos, char &now, int &state);            //当字符为&时的处理
void ProcessSlashSymbol(string s, int &pos, char &now, int &state, int &flag);//当字符为/时的处理

int main()
{
    char buffer[1024];
    string buf;
    FILE *fptr;
    //如果打开文件失败
    if ((fptr = fopen("Input.txt", "r")) == NULL)
    {
        printf("Error! opening file");
    }
    //对每一行分别处理
    while (fgets(buffer, sizeof(buffer), fptr) != NULL)
    {
        NumOfLine++;
        buf = buffer;
        wordAnalysis(buffer);
    }
    OutputStatistics();
    return 0;
}

bool isLetter(char ch)
{
    if (ch >= 'a' && ch <= 'z')
        return true;
    else
        return false;
}

bool isFigure(char ch)
{
    if (ch >= '0' && ch <= '9')
        return true;
    else
        return false;
}

void wordAnalysis(string s)
{
    int state = 0;
    int pos = 0;
    int posVar;
    char now = ' ';
    char var[200]; //用于暂存标识符或关键字
    string variable;
    if (flag)
        state = 3;
    while (now != '\0')
    {
        //printf("now is %d or '%c', state is %d\n", now, now, state);
        switch (state)
        {
        case 0:
            now = s[pos];
            if (isLetter(now))
                state = 1;
            else if (isFigure(now))
                state = 2;
            else
                state = 3;
            break;
        case 1: //如果是字母开头，有标识符和关键字两种情况
            LetterSituation(s, pos, now);
            state = 0;
            break;
        case 2: //如果开头是数字
            FigureSituation(s, pos, now);
            state = 0;
            break;
        case 3:
            SymbolSituation(s, pos, now, state, flag);
            break;
        default:
            break;
        }
    }
}

void LetterSituation(string s, int &pos, char &now)
{
    int posVar;
    char var[200]; //用于暂存标识符或关键字
    string variable;
    memset(var, 0, sizeof(var));
    posVar = 0;
    while (isLetter(now) || isFigure(now) || now == '_')
    {
        var[posVar++] = now;
        now = s[++pos];
    }
    if (symbols.find(s[pos]) == symbols.end()) //错误处理1：如果变量申明不合法即报错
    {
        cout << "Line " << NumOfLine << ": error! There is an illegal variable name!" << endl;
        now = '\0'; //有错误的这一行直接略过
    }
    else //如果变量申明合法
    {
        var[posVar] = '\0';
        variable = var;
        //如果是关键词，输出
        if (keywords.find(variable) != keywords.end())
        {
            printPair(var, "Keyword");
            NumOfKeyWord++;
        }
        //如果是变量，先输出，判断是否在符号表里，在则不做处理
        //如果不在，加进符号表里
        else
        {
            printPair(var, "Variable");
            NumOfVarible++;
            if (!SymbolTable.count(variable))
            {
                SymbolTable.emplace(variable, NumOfLine);
            }
        }
    }
}
void FigureSituation(string s, int &pos, char &now)
{
    int posVar;
    char var[200]; //用于暂存标识符或关键字
    string variable;
    memset(var, 0, sizeof(var));
    posVar = 0;
    while (isFigure(now))
    {
        var[posVar++] = now;
        now = s[++pos];
    }
    if (now == '.') //如果该数字含有小数点
    {
        var[posVar++] = now;
        now = s[++pos];
        while (isFigure(now))
        {
            var[posVar++] = now;
            now = s[++pos];
        }
    }
    if (symbols.find(s[pos]) == symbols.end()) //错误处理2：如果常数申明不合法即报错
    {
        cout << "Line " << NumOfLine << ": error! There is an illegal constant number!" << endl;
        now = '\0'; //有错误的这一行直接略过
    }
    else //如果变量申明合法
    {
        var[posVar] = '\0';
        variable = var;
        printPair(var, "Number");
        NumOfNumber++;
    }
}
void ProcessOtherSymbols(string s, int &pos, char &now, int &state)
{
    pos++;
    state = 0;
    now = s[pos];
}
void ProcessSingleQuotationMarks(string s, int &pos, char &now, int &state)
{
    pos += 2; //位置移到单引号后面两个位置
    now = s[pos];
    if (now != '\'') //错误处理4：若一行中没有找到对应的单引号，则报错
    {
        cout << "Line " << NumOfLine << ": error! There is no matching single colon." << endl;
        state = 0;
    }
}
void ProcessDoubleQuotationMarks(string s, int &pos, char &now, int &state)
{
    int posVar;
    char var[200]; //用于暂存标识符或关键字
    string variable;
    memset(var, 0, sizeof(var));
    posVar = 0;
    now = s[++pos];
    while (now != '"' && now != '\0')
    {
        var[posVar++] = now;
        now = s[++pos];
    }
    variable = var;
    if (now == '"')
    {
        printPair(variable, "string");
        pos++;
        state = 0;
        now = s[pos];
    }
    else //错误处理3：若一行中没有找到对应的双引号，则报错
    {
        cout << "Line " << NumOfLine << ": error! There is no matching double colon." << endl;
        state = 0;
    }
}
void ProcessBoundarySymbol(string s, int &pos, char &now, int &state)
{
    string variable;
    NumOfBound++;
    variable = now;
    printPair(variable, "boundary");
    pos++;
    state = 0;
    now = s[pos];
}
void ProcessCompareSymbol(string s, int &pos, char &now, int &state)
{
    string variable;
    //对于以上符号有两种情况，单独符号和在后面加等号的情况（如 >=, ==, !=）
    //对于后面加等号的情况需要另判断
    if (s[pos + 1] != '=')
    {
        variable = now;
        printPair(variable, "operator");
    }
    else
    {
        variable = now;
        variable += "=";
        printPair(variable, "operator");
        pos++;
    }
    NumOfOperator++;
    pos++;
    state = 0;
    now = s[pos];
}
void ProcessPlusSymbol(string s, int &pos, char &now, int &state)
{
    string variable;
    if (s[pos + 1] == '=') //"+=" 的情况
    {
        variable = now;
        variable += "=";
        printPair(variable, "operator");
        pos++;
    }
    else if (s[pos + 1] == '+') //"++" 的情况
    {
        variable = now;
        variable += "+";
        printPair(variable, "operator");
        pos++;
    }
    else
    {
        variable = now;
        printPair(variable, "operator");
    }
    NumOfOperator++;
    pos++;
    state = 0;
    now = s[pos];
}
void ProcessMinusSymbol(string s, int &pos, char &now, int &state)
{
    string variable;
    if (s[pos + 1] == '=')
    { //"-=" 的情况
        variable = now;
        variable += "=";
        printPair(variable, "operator");
        pos++;
    }
    else if (s[pos + 1] == '-') //"--" 的情况
    {
        variable = now;
        variable += "-";
        printPair(variable, "operator");
        pos++;
    }
    else
    {
        variable = now;
        printPair(variable, "operator");
    }
    NumOfOperator++;
    pos++;
    state = 0;
    now = s[pos];
}
void ProcessLineSymbol(string s, int &pos, char &now, int &state)
{
    string variable;
    if (s[pos + 1] == '|')
    { //"||" 的情况
        variable = now;
        variable += "|";
        printPair(variable, "operator");
        pos++;
    }
    else
    {
        variable = now;
        printPair(variable, "operator");
    }
    NumOfOperator++;
    pos++;
    state = 0;
    now = s[pos];
}
void ProcessAndSymbol(string s, int &pos, char &now, int &state)
{
    string variable;
    if (s[pos + 1] == '&')
    { //"&&" 的情况
        variable = now;
        variable += "&";
        printPair(variable, "operator");
        pos++;
    }
    else
    {
        variable = now;
        printPair(variable, "operator");
    }
    NumOfOperator++;
    pos++;
    state = 0;
    now = s[pos];
}
void ProcessSlashSymbol(string s, int &pos, char &now, int &state, int &flag)
{
    string variable;
    if (flag)
        goto annotation;
    //对于后面加等号的情况需要另判断，表示除法
    if (s[pos + 1] == '=')
    {
        variable = now;
        variable += "=";
        printPair(variable, "operator");
        NumOfOperator++;
        pos += 2;
    }
    else if (s[pos + 1] == '/')
    { //表示“//”的注释
        cout << "Line " << NumOfLine << ": single-lined annotation" << endl;
        now = '\0';
    }
    else if (s[pos + 1] == '*')
    { //表示“/*”的注释
        flag = 1;
        pos += 2;
    annotation:
        while (!(s[pos] == '*' && s[pos + 1] == '/') && s[pos] != '\0')
        {
            pos++;
        }
        if (s[pos] != '\0') //如果成功找到*/
        {
            cout << "Line " << NumOfLine << ": long annotation" << endl;
            flag = 0;
            pos += 2;
        }
    }
    else
    { //如果只表示除
        NumOfOperator++;
        variable = now;
        printPair(variable, "operator");
        pos++;
    }
}
void SymbolSituation(string s, int &pos, char &now, int &state, int &flag)
{
    int posVar;
    char var[200]; //用于暂存标识符或关键字
    string variable;
    memset(var, 0, sizeof(var));
    posVar = 0;
    if (flag)
        goto annotation;
    switch (now)
    {
    //下面处理其他符号，遇到这些符号则直接略过
    case ' ':
    case ',':
    case ':':
    case ';':
    case '\n':
    case '?':
    {
        ProcessOtherSymbols(s, pos, now, state);
        break;
    }
    //双引号和单引号都需要单独处理，直到找到其后面的匹配为止
    case '"':
    {
        ProcessDoubleQuotationMarks(s, pos, now, state);
        break;
    }
    case '\'':
    {
        ProcessSingleQuotationMarks(s, pos, now, state);
        break;
    }
    case '#': //有#为宏定义，改行直接跳过
    {
        now = '\0';
        break;
    }
    //下面是对界符的处理
    case '[':
    case ']':
    case '(':
    case ')':
    case '{':
    case '}':
    {
        ProcessBoundarySymbol(s, pos, now, state);
        break;
    }
    //下面是对运算符的处理
    case '>':
    case '<':
    case '=':
    case '!':
    case '*':
    case '%':
    {
        ProcessCompareSymbol(s, pos, now, state);
        break;
    }
    case '+':
    {
        ProcessPlusSymbol(s, pos, now, state);
        break;
    }
    case '-':
    {
        ProcessMinusSymbol(s, pos, now, state);
        break;
    }
    case '|':
    {
        ProcessLineSymbol(s, pos, now, state);
        break;
    }
    case '&':
    {
        ProcessAndSymbol(s, pos, now, state);
        break;
    }
    case '/': //对斜杠需要特判，判断后面是否有注释
    {
    annotation:
        ProcessSlashSymbol(s, pos, now, state, flag);
        state = 0;
        break;
    }
    default: //对于其他不可识别符号
        cout << "unidentified symbol : " << now << endl;
        pos++;
        state = 0;
        break;
    }
}

void OutputStatistics()
{
    if (SymbolTable.empty())
    {
        cout << "符号表为空" << endl;
    }
    else
    {
        cout << "符号表:" << endl;
        for (auto iter = SymbolTable.begin(); iter != SymbolTable.end(); ++iter)
        {
            cout << "At Line " << iter->second << ": " << iter->first << endl;
        }
    }
    cout << "行数：" << NumOfLine << endl;
    cout << "关键词出现的次数：" << NumOfKeyWord << endl;
    cout << "变量出现的次数：" << NumOfVarible << endl;
    cout << "数字常量出现的次数：" << NumOfNumber << endl;
    cout << "运算符出现的次数：" << NumOfOperator << endl;
    cout << "界符出现的次数：" << NumOfBound << endl;
}

void printPair(string sym, string attrtibute)
{
    cout << "Line " << NumOfLine << ": <" << sym << ", " << attrtibute << ">" << endl;
}

