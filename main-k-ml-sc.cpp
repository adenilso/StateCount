
#include "FSM.h"
#include "Graph.h"
#include "BigNumber.h"
#include <queue>
#include <set>
#include <iterator>
#include <iostream>
#include <fstream>

void yyparse(void);

FSM fsm;

void usage() {
  cout << "usage: sc numOuts numInps numStates numVariants seed\n";
  exit(1);
}

int main(int argc, char * argv[]) {
  yyparse();
  if (argc < 1) {
    cout << argc << endl;
    usage();
  }
  long numfsms = atol(argv[1]);
  fsm.setup();
  GraphNode::setNumChildren(fsm.numInputs);
  cout << "INFO>BEGIN FSM" << endl;
  fsm.printOn(cout, "FSM>");
  fsm.printDotOn(cout, "DOT>");
//  graphSC.printWithoutPrefix("seq sc:");
  cout << "INFO>END FSM" << endl;
  BigNumber numNQE = 0;
  BigNumber numQE = 0;
  int minDistance = 10000;
  int maxDistance = -10000;
  map<int,pair<BigNumber, BigNumber> > distStatsTC;
  map<int,pair<BigNumber, BigNumber> > distStatsRSC;
  map<int,pair<BigNumber, BigNumber> > distStatsSC;
  set<BigNumber> generatedFSMs;
  BigNumber numToBeGenerated = numfsms;
  for (int j = 0; j < numfsms; j++) {
    FSM m1;
    m1.generateRandomFull(fsm.getNumStates(), fsm.intToState, fsm.getNumInputs(), fsm.intToInput, fsm.getNumOutputs(), fsm.intToOutput);
    m1.setup();
    int dist = fsm.shortestSequenceDistance(m1); 
    cout << "INFO>BEGIN FSM" << endl;
    m1.printOn(cout, "FSM>");
    m1.printDotOn(cout, "DOT>");
    cout << ">>>length of min killing test: " << dist << endl;
    cout << "INFO>END FSM" << endl;
  }
}
