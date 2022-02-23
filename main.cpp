
#include "FSM.h"
#include "Graph.h"
#include <set>
#include <iterator>
#include <iostream>
#include <fstream>

void yyparse(void);

FSM fsm;

void printInfo(ostream &os, FSM &m) {
  os << "INFO>BEGIN FSM" << endl;
  os << "INFO>Number of Transitions: " << m.getNumTransitions() << " (" << (int) ((100*(m.getNumTransitions())) / (m.getNumStates() * m.getNumInputs())) << "%)" << endl;
  m.printOn(os, "FSM>");
  m.printDotOn(os, "DOT>");
  Graph graphTC(m);
  graphTC.spanTransitionCover();
  os << "INFO>Total Length for TC: " << graphTC.totalLength() << endl;
  Graph graphRSC(m);
  graphRSC.spanReducedStateCounting();
  os << "INFO>Total Length for RSC: " << graphRSC.totalLength() << endl;
  int numSeparable = 0;
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
}

void usage() {
  cout << "usage: sc numOuts numInps numStates numVariants seed\n";
  exit(1);
}

int main(int argc, char * argv[]) {
//  yyparse();
  if (argc < 6) {
    cout << argc << endl;
    usage();
  }
  int no = atoi(argv[1]);
  int ni = atoi(argv[2]);
  int ns = atoi(argv[3]);
  int variants = atoi(argv[4]);
  int seed = atoi(argv[5]);
  srand(seed);
  fsm.generateRandom(ns, ni, no);
  fsm.setup();
  GraphNode::setNumChildren(fsm.numInputs);
  for (int j = 0; j < variants; j++) {
    FSM m1(fsm);
  cout << 78 << endl;
    printInfo(cout, m1);

    for (int i = ns - 1; i < ns * ni; i++) {
      m1.addRandomTransition();
      m1.setup();
//      m1.debug(cout);
      printInfo(cout, m1);
    }
  }
}

