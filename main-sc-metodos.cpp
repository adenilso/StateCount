
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
  int k = 1;
  yyparse();
  fsm.setup();
  fsm.debug(cout);
  GraphNode::setNumChildren(fsm.getNumStates());

  Graph graphSC(fsm);
  int m = fsm.getNumStates();
  if (argc > 1) {
    m = atoi(argv[1]);
  }
  if (argc > 2) {
    k = atoi(argv[2]);
  }

  cout << ">>>" << "Number of States in Implementation FSMs (m): " << m << endl;
  cout << "STATE COUNTING (SC)" << endl;
  cout << "*******************" << endl;
  graphSC.spanStateCounting(m);
  graphSC.printWithoutPrefix(">>> Result Seq:");
  cout << ">>>" << "Total Length for SC: " << graphSC.totalLength() << endl;
  cout << "****************************" << endl;

  Graph graphTC(fsm);
  graphTC.spanTransitionCover();
  cout << "TRANSITION COVER (TC)" << endl;
  cout << "*********************" << endl;
  graphTC.printWithoutPrefix(">>> Result Seq:");
  cout << ">>>" << "Total Length for TC: " << graphTC.totalLength() << endl;
  cout << "****************************" << endl;

  Graph graphRSC(fsm);
  graphRSC.spanReducedStateCounting();
  cout << "REDUCED STATE COUNTING (RSC)" << endl;
  cout << "****************************" << endl;
  graphRSC.printWithoutPrefix(">>> Result Seq:");
  cout << "INFO>Total Length for RSC: " << graphRSC.totalLength() << endl;
  cout << "****************************" << endl;

  Graph graphRSCK(fsm);
  graphRSCK.spanReducedStateCountingK(k);
  cout << "REDUCED STATE COUNTING (RSCK) k = "<< k << endl;
  cout << "*************************************" << endl;
  graphRSCK.printWithoutPrefix(">>> Result Seq:");
  cout << "INFO>Total Length for RSCK: " << graphRSCK.totalLength() << endl;
  cout << "****************************" << endl;


/*  int numSeparable = 0;
  map<pair<int, int>, vector<int> > & g = m.gamma;
  for (int i = 0; i < m.getNumStates(); i++) {
    for (int j = 0; j < m.getNumStates(); j++) {
      if (i < j) {
        const vector<int> &seq = g[make_pair(i, j)];
        if (seq.size() > 0) {
          numSeparable ++;
        }
      }
    }
  }
  os << "INFO>Number of Separable Pairs: " << numSeparable << endl;
  os << "INFO>";
  for (int i = 0; i < m.getNumStates(); i++) {
    for (int j = 0; j < m.getNumStates(); j++) {
      if (i < j) {
        const vector<int> &seq = g[make_pair(i, j)];
        if (seq.size() > 0) {
          os << " gamma(" << m.intToState[i] << ", " << m.intToState[j] << ") = ";
          for (vector<int>::const_iterator it = seq.begin(); it != seq.end(); it++) {
            os << m.intToInput[*it] << " ";
          }
        }
      }
    }
  }
  os << endl;
  Graph graphSC(m);
  graphSC.spanStateCounting(m.getNumStates());
  os << "INFO>Total Length for SC: " << graphSC.totalLength() << endl;
  os << "INFO>END FSM" << endl;

*/


}

