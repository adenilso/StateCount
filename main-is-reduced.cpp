//responde se a mef passada como parametro eh reduzida ou nao.
#include "FSM.h"
#include "Graph.h"
#include <set>
#include <iterator>
#include <iostream>
#include <fstream>

void yyparse(void);

FSM fsm;

void usage() {
  cout << "usage: fsm-is-reduced < mef.txt \n";
  exit(1);
}

int main(int argc, char * argv[]) {
  yyparse();
  fsm.setup();
//  fsm.debug(cout);
  GraphNode::setNumChildren(fsm.getNumStates());
  //Graph graphSC(fsm);
  int m = fsm.getNumStates();
  
  if (fsm.isReduced()) {			//se a fsm for reduzida
    cout << ">>>" << " FSM EH REDUZIDA " << endl;
  }
  else{
	cout << ">>>" << " FSM NAO EH REDUZIDA " << endl;  
	  }	  
 
}
