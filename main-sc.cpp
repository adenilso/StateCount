
#include "FSM.h"
#include "Graph.h"
#include <set>
#include <iterator>
#include <iostream>
#include <fstream>

void yyparse(void);

FSM fsm;

void usage() {
  cout << "usage: sc m\n";
  exit(1);
}

int main(int argc, char * argv[]) {
  yyparse();
  fsm.setup();
  fsm.debug(cout);
  GraphNode::setNumChildren(fsm.getNumStates());
  Graph graphSC(fsm);
  int m = fsm.getNumStates();
  if (argc > 1) {
    m = atoi(argv[1]);
  }
  cout << ">>>" << "Number of States in Implementation FSMs (m): " << m << endl;
  graphSC.spanStateCounting(m);
  graphSC.printWithoutPrefix(">>> Result Seq:");
  cout << ">>>" << "Total Length for SC: " << graphSC.totalLength() << endl;
}

