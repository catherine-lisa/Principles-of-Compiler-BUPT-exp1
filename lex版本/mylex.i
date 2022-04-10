/*
* @project: wordAnalysis(lex version)
* @IDE:    Visual Studio
* @Author: Jiang Shutong
* @Date:   Oct 10th
*/

%{
#include <stdio.h> 
#include <string.h>
/* 比较运算符 */
#define LT					1
#define	LE					2
#define GT					3
#define	GE					4
#define	EQ					5
#define PL                  6
#define PP                  7
#define PE                  8
#define MS                  8
#define MM                  9
#define ME                  10
/* 保留字 */
#define IF  				11
#define ELSE				12
#define RETURN				13
#define BREAK				14
#define WHILE				15
#define	DO					16
#define ID                  17
#define NUMBER              18
#define RELOP               19
#define DELIMITER           20
/* 其他情况 */
#define ERRORCHAR           21
 
int yylval;
char symbolTable[1024][1024];
int loc = 0;
%}

delim		[ \t \n]
ws			{delim}+
letter   	[A-Za-z_]
digit		[0-9]
id			{letter}({letter}|{digit})*
number  	{digit}+(\.{digit}+)?
 
%%

{ws}	          {;/* 此时词法分析器没有动作，也不返回，而是继续分析。 */}
while	          {return (WHILE);}
do		          {return (DO);}
if                {return (IF);}
else              {return (ELSE);}
return            {return (RETURN);}
break             {return (BREAK);}
{id}	          {yylval = installID (); return (ID);}
{number}	      {return (NUMBER);}
"<"	              {yylval = LT; return (RELOP);}
"<="	          {yylval = LE; return (RELOP);}
"="	              {yylval = EQ; return (RELOP);}
">"	              {yylval = GT; return (RELOP);}
">="	          {yylval = GE; return (RELOP);}
"+" 	          {yylval = PL; return (RELOP);}
"++"	          {yylval = PP; return (RELOP);}
"+="	          {yylval = PE; return (RELOP);}
"-" 	          {yylval = MS; return (RELOP);}
"--"	          {yylval = MM; return (RELOP);}
"-="	          {yylval = ME; return (RELOP);}
"{"               {return (DELIMITER);}
"}"               {return (DELIMITER);}
"("               {return (DELIMITER);}
")"               {return (DELIMITER);}
";"               {return (DELIMITER);}

.				  {yylval = ERRORCHAR; return ERRORCHAR;}
 /*.匹配除换行之外的任何字符，一般可作为最后一条翻译规则。*/
 
%%

int installID () {
    for(int i = 0; i < loc; i ++){
        if(strcmp(symbolTable[i], yytext) == 0) //如果符号表中已经存在
        return ID;
    }
    strcpy(symbolTable[loc], yytext);
    //printf("at loc %d, symbol is %s", loc, symbolTable[loc]);
    loc ++;
	return ID;
}
 
 
int yywrap (){
  return 1;
}
 
void writeout(int c){
  switch(c){
  	case ERRORCHAR: fprintf(yyout, "(ERRORCHAR, \"%s\")\n", yytext);break;
  	case RELOP: fprintf(yyout, "(RELOP, \"%s\") \n", yytext);break;  	
  	case DELIMITER: fprintf(yyout, "(DELIMITER, \"%s\") \n", yytext);break;  	  
    case WHILE: fprintf(yyout, "(WHILE, \"%s\")\n", yytext);break;
    case DO: fprintf(yyout, "(DO, \"%s\") \n", yytext);break;
    case IF: fprintf(yyout, "(IF, \"%s\") \n", yytext);break;
    case ELSE: fprintf(yyout, "(ELSE, \"%s\") \n", yytext);break;
    case RETURN: fprintf(yyout, "(RETURN, \"%s\") \n", yytext);break;
    case BREAK: fprintf(yyout, "(BREAK, \"%s\") \n", yytext);break;
    case NUMBER: fprintf(yyout, "(NUM, \"%s\")\n", yytext);break;
    case ID: fprintf(yyout, "(ID, \"%s\")\n", yytext);break;
    default:break;
  }
  return;
}
 
 
int main (int argc, char ** argv){
	int c,j=0;
	if (argc>=2){
	  if ((yyin = fopen(argv[1], "r")) == NULL){
	    printf("Can't open file %s\n", argv[1]);
	    return 1;
	  }
	  if (argc>=3){
	    yyout=fopen(argv[2], "w");
	  }
	}
	while (c = yylex()){
		writeout(c);
		j++;
	}
	if(argc>=2){
	  fclose(yyin);
	  if (argc>=3) fclose(yyout);
	}

    printf("The symbolTable is listed as follows:\n");
    for(int i = 0; i < loc; i ++){
        printf("the %d symbol is: %s\n", i, symbolTable[i]);
    }
	return 0;
}