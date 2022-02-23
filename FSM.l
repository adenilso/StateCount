
%{
#include<string>

#include "FSM.tab.hpp"

extern char * yytext;
%}

%option noyywrap

%%
[ \t\n] { }
"--"    {return HH;}
"->"    {return HG;}
"/"    {return SL;}
";"    {return SC;}
[0-9a-zA-Z]+ { 
  yylval.str = new std::string(yytext); 
  return ID; 
}
%%

