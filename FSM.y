%{
#include <string>
%}

%union{
  std::string *str;
}

%token HH
%token HG
%token <str> ID
%token SL
%token SC

%type <str> state;
%type <str> input;
%type <str> output;

%{

#include "FSM.h"

extern "C" {
int yyerror(const char *s);
}
int yylex();

extern FSM fsm;

%}

%%
fsm : 
  transitions
  {
    fsm.endUpTransitions();
  }
  ;

transitions : 
  transition
  | transitions transition
  ;

transition :
  state HH input SL output HG state semicolon_or_nothing
  {
    fsm.addTempTransition(*$1, *$3, *$5, *$7);
  }
  ;

state : 
  ID 
  ;

input : 
  ID 
  ;

output : 
  ID 
  ;

semicolon_or_nothing :
  SC
  |
  ;

%%

int yyerror(const char *s) {
  return 1;
}
