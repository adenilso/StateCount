
#include "FSM.h"
#include "Statistics.h"
#include "Graph.h"
#include "Statistics.h"
#include <values.h>
#include <math.h>
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
  char * stopFile = argv[1];
  fsm.setup();
  long kSC = 0;
  long kTC = 0;
  long kRSC = 0;
  long numGenerated = 0;
  GraphNode::setNumChildren(fsm.numInputs);
  cout << "INFO>BEGIN FSM" << endl;
  cout << "INFO>Number of States: " << fsm.getNumStates() << endl;
  cout << "INFO>Number of Inputs: " << fsm.getNumInputs() << endl;
  cout << "INFO>Number of Outputs: " << fsm.getNumOutputs() << endl;
  cout << "INFO>Number of Transitions: " << fsm.getNumTransitions() << " (" << (int) ((100*(fsm.getNumTransitions())) / (fsm.getNumStates() * fsm.getNumInputs())) << "%)" << endl;
  fsm.printOn(cout, "FSM>");
  fsm.printDotOn(cout, "DOT>");
  Graph graphTC(fsm);
  graphTC.spanTransitionCover();
  Graph graphRSC(fsm);
  graphRSC.spanReducedStateCounting();
  int numSeparable = 0;
  map<pair<int, int>, vector<int> > & g = fsm.gamma;
  for (int i = 0; i < fsm.getNumStates(); i++) {
    for (int j = 0; j < fsm.getNumStates(); j++) {
      if (i < j) {
        const vector<int> &seq = g[make_pair(i, j)];
        if (seq.size() > 0) {
          numSeparable ++;
        }
      }
    }
  }
  Graph graphSC(fsm);
  graphSC.spanStateCounting(fsm.getNumStates());
  cout << "INFO>END FSM" << endl;
  long numNQE = 0;
  long numQE = 0;
  vector<int> nkTC;
  vector<int> nkRSC;
  vector<int> nkSC;
  vector<int> qeFSM;
  Statistics distStatistics;
  map<int,Statistics> distStatsTC;
  map<int,Statistics> distStatsRSC;
  map<int,Statistics> distStatsSC;
  FSM m1(fsm);
  m1.initFSM();
  double dblNumToBeGenerated = pow((double)fsm.getNumStates() * fsm.getNumOutputs(), (double)fsm.getNumStates() * fsm.getNumInputs()); 
  if (dblNumToBeGenerated > (double) MAXLONG) {
    cout << ">>> The number of FSMs (" << dblNumToBeGenerated << ") that would be generated exceeds the machine capability.<BR>\n" << endl;
    exit(1);
  }
  map<int, set<GraphNode *> > kMutAdequate;
  long numToBeGenerated = (long) dblNumToBeGenerated;
  cout << ">>>" << numToBeGenerated << " FSMs will be generated<BR>\n";
  while (m1.incFSM()) {
    numGenerated++;
    m1.setup();
    int dist = fsm.outputDistance(m1); 
    distStatistics.addValue(dist);
    GraphNode * sckseq  = NULL;
    if (!fsm.isQuasiEq(m1)) {
      long eq;
      long df;
      sckseq = graphSC.compareOutput(fsm, m1, eq, df, true);
      if (df > 0) {
        kSC++;
        kMutAdequate[dist].insert(sckseq);
      } else {
        nkSC.push_back(numGenerated);
      }
      numNQE++;
    } else {
      qeFSM.push_back(numGenerated);
      numQE++;
    }
    distStatsSC[dist].addValue(sckseq != NULL);
    if (numGenerated % 10000 == 10000 - 1 || numGenerated == numToBeGenerated) {
      cout << ">>><HR>" << endl;
      cout << ">>>Number of Mutant FSMs: " << numGenerated << " of " << numToBeGenerated << " (" << 100.0 * numGenerated / numToBeGenerated << "%)" << endl;
      if (stopFile != NULL) {
	      bool flag = false;
	      fstream fin;
	      fin.open(stopFile,ios::in);
	      if( fin.is_open() ) {
	        break;
	      }
	      fin.close();
      }
    }
  }
  map<int, Graph *> kMutAdequateSeqs;
  for (int d1 = 1; d1 <= (int) distStatistics.getMax(); d1++) {
    for (int d2 = d1 + 1; d2 <= (int) distStatistics.getMax(); d2++) {
      copy(kMutAdequate[d1].begin(), kMutAdequate[d1].end(), 
          insert_iterator<set<GraphNode *> > (kMutAdequate[d2], kMutAdequate[d2].end()));
    }
  }
  for (int d = 1; d <= (int) distStatistics.getMax(); d++) {
    Graph * g = new Graph(fsm);
    GraphNode * p = g->setRoot(fsm.initialState);
    const set<GraphNode *> seqs = kMutAdequate[d];
    vector<int> seq;
    for (set<GraphNode *>::const_iterator it = seqs.begin(); it != seqs.end(); it++) {
      seq.clear();
      graphSC.getInputSeq(*it, seq);
      g->addNode(p, seq);
    }
    cout << "-----------------------" << endl;
    cout << "Distance " << d << endl;
    g->printWithoutPrefix("");
    kMutAdequateSeqs[d] = g;
  }
  m1.initFSM();
  map<int, long> kSCkMut;
  while (m1.incFSM()) {
    if (!fsm.isQuasiEq(m1)) {
      for (int d = 1; d <= (int) distStatistics.getMax(); d++) {
        long eq;
        long df;
        kMutAdequateSeqs[d]->compareOutput(fsm, m1, eq, df, true);
        if (df > 0) {
          kSCkMut[d]++;
        }
      }
    }
  }
  for (int d = 1; d <= (int) distStatistics.getMax(); d++) {
    cout << " distance " << d << " killed " << kSCkMut[d] << " of " << numNQE << endl;
  }
  for (int d = 1; d <= (int) distStatistics.getMax(); d++) {
    delete kMutAdequateSeqs[d];
  }
}

