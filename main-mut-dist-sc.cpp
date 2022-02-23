
#include "FSM.h"
#include "Graph.h"
#include "Statistics.h"
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
//  yyparse();
  if (argc < 6) {
    cout << argc << endl;
    usage();
  }
  int no = atoi(argv[1]);
  int ni = atoi(argv[2]);
  int ns = atoi(argv[3]);
  int nt = atoi(argv[4]);
  int numfsms = atoi(argv[5]);
  int distType = atoi(argv[6]);
  int seed = atoi(argv[7]);
  int kSC = 0;
  int kTC = 0;
  int kRSC = 0;
  int numGenerated = 0;
  srand(seed);
  fsm.generateRandom(ns, ni, no);
  for (int i = ns - 1; i < nt; i++) {
   fsm.addRandomTransition();
  }
  fsm.setup();
  GraphNode::setNumChildren(fsm.numInputs);
  cout << "INFO>BEGIN FSM" << endl;
  cout << "INFO>Number of Transitions: " << fsm.getNumTransitions() << " (" << (int) ((100*(fsm.getNumTransitions())) / (fsm.getNumStates() * fsm.getNumInputs())) << "%)" << endl;
  fsm.printOn(cout, "FSM>");
  fsm.printDotOn(cout, "DOT>");
  Graph graphTC(fsm);
  graphTC.spanTransitionCover();
  cout << "INFO>Total Length for TC: " << graphTC.totalLength() << endl;
  graphTC.printWithoutPrefix("seq tc:");
  Graph graphRSC(fsm);
  graphRSC.spanReducedStateCounting();
  cout << "INFO>Total Length for RSC: " << graphRSC.totalLength() << endl;
  graphRSC.printWithoutPrefix("seq rsc:");
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
  cout << "INFO>Number of Separable Pairs: " << numSeparable << endl;
  cout << "INFO>";
  for (int i = 0; i < fsm.getNumStates(); i++) {
    for (int j = 0; j < fsm.getNumStates(); j++) {
      if (i < j) {
        const vector<int> &seq = g[make_pair(i, j)];
        if (seq.size() > 0) {
          cout << " gamma(" << fsm.intToState[i] << ", " << fsm.intToState[j] << ") = ";
          for (vector<int>::const_iterator it = seq.begin(); it != seq.end(); it++) {
            cout << fsm.intToInput[*it] << " ";
          }
        }
      }
    }
  }
  cout << endl;
  Graph graphSC(fsm);
  graphSC.spanStateCounting(fsm.getNumStates());
  cout << "INFO>Total Length for SC: " << graphSC.totalLength() << endl;
  graphSC.printWithoutPrefix("seq sc:");
  cout << "INFO>END FSM" << endl;
  int numNQE = 0;
  vector<int> nkTC;
  vector<int> nkRSC;
  vector<int> nkSC;
  vector<int> qeFSM;
  Statistics distStatistics;
  map<int,Statistics> distStatsTC;
  map<int,Statistics> distStatsRSC;
  map<int,Statistics> distStatsSC;
  vector<FSM *> generatedfsms;
  for (int j = 0; j < numfsms; j++) {
    FSM * m1 = new FSM();
    m1->generateRandomFull(ns, ni, no);
    m1->setup();
    bool newFSM = true;
    for(vector<FSM *>::const_iterator it = generatedfsms.begin(); it != generatedfsms.end(); it++) {
      if (m1->isQuasiEq(**it)) {
        newFSM = false;
      }
    }
    if (newFSM) {
      numGenerated++;
      generatedfsms.push_back(m1);
#ifdef DEBUGVERSION
      cout << "INFO>BEGIN MUTANT FSM" << endl;
      m1->printOn(cout, "FSM>");
      m1->printDotOn(cout, "DOT>");
#endif
      int dist;
      if (distType == OUTPUTDIST) {
        dist = fsm.outputDistance(*m1); 
      } else if (distType == LENMINDIST) {
        dist = fsm.shortestSequenceDistance(*m1);
      }
      distStatistics.addValue(dist);
#ifndef DEBUGVERSION
      cout << ">>> distance: " << dist << endl;
#endif
      if (!fsm.isQuasiEq(*m1)) {
#ifndef DEBUGVERSION
        cout << ">>><B> It is not quasi-eq to M0 </B>" << endl;
#endif
        long eq;
        long df;
        GraphNode * tckseq = graphTC.compareOutput(fsm, *m1, eq, df, true);
        if (df > 0) {
          kTC++;
#ifdef DEBUGVERSION
          if (tckseq != NULL) {
            cout << ">>>killed by <b>tc" << eq << "</b>:";
            graphTC.printSeqFromLast("", tckseq);
            cout << endl;
          }
#endif
        } else {
#ifdef DEBUGVERSION
          cout << ">>><b> Not Killed by TC</b>" << endl;
#endif
          nkTC.push_back(numGenerated);
        }
        distStatsTC[dist].addValue(tckseq != NULL);
        GraphNode * rsckseq = graphRSC.compareOutput(fsm, *m1, eq, df, true);
        if (df > 0) {
          kRSC++;
#ifdef DEBUGVERSION
          if (rsckseq != NULL) {
            cout << ">>>killed by <b>rsc" << eq << "</b>:";
            graphRSC.printSeqFromLast("", rsckseq);
            cout << endl;
          }
#endif
        } else {
#ifdef DEBUGVERSION
          cout << ">>><b> Not Killed by RSC</b>" << endl;
#endif
          nkRSC.push_back(numGenerated);
        }
        distStatsRSC[dist].addValue(rsckseq != NULL);
        GraphNode * sckseq = graphSC.compareOutput(fsm, *m1, eq, df, true);
        if (df > 0) {
          kSC++;
#ifdef DEBUGVERSION
          if (sckseq != NULL) {
            cout << ">>>killed by <b>sc" << eq << "</b>:";
            graphSC.printSeqFromLast("", sckseq);
            cout << endl;
          }
#endif
        } else {
#ifdef DEBUGVERSION
          cout << ">>><b> Not Killed by SC</b>" << endl;
#endif
          nkSC.push_back(numGenerated);
        }
        distStatsSC[dist].addValue(sckseq != NULL);
        numNQE++;
#ifdef DEBUGVERSION
        cout << "INFO>END MUTANT FSM" << endl;
#endif
      } else {
#ifdef DEBUGVERSION
        cout << ">>><B> It is quasi-eq to M0 </B>" << endl;
#endif
        qeFSM.push_back(numGenerated);
      }
    } else {
      delete m1;
    }
  }
  cout << ">>><HR>" << endl;
  cout << ">>>Number of Mutant FSMs: " << numGenerated << endl;
  cout << ">>>Number of Quasi EQ FSMs: " << numGenerated - numNQE << endl;
  if (numNQE > 0) {
    cout << ">>>Percentage of Mutant FSMs killed by TC seqs: " << 100 * kTC / numNQE << "%" << endl;
    if (nkTC.size() > 0) {
      cout << ">>>Number of the FSM Kept alive: ";
      copy(nkTC.begin(), nkTC.end(), ostream_iterator<int>(cout, " "));
      cout << endl;
    }
    cout << ">>>Percentage of Mutant FSMs killed by RSC seqs: " << 100 * kRSC / numNQE << "%" << endl;
    if (nkRSC.size() > 0) {
      cout << ">>>Number of the FSM Kept alive: ";
      copy(nkRSC.begin(), nkRSC.end(), ostream_iterator<int>(cout, " "));
      cout << endl;
    }
    cout << ">>>Percentage of Mutant FSMs killed by SC seqs: " << 100 * kSC / numNQE << "%" << endl;
    if (nkSC.size() > 0) {
      cout << ">>>Number of the FSM Kept alive: ";
      copy(nkSC.begin(), nkSC.end(), ostream_iterator<int>(cout, " "));
      cout << endl;
    }
  }
  for(int d = (int) distStatistics.getMin(); d <= (int) distStatistics.getMax(); d++) {
    cout << ">>>Distance: " << d;
    cout << " num TC: " << distStatsTC[d].getNum();
    cout << " min TC: " << distStatsTC[d].getMin();
    cout << " max TC: " << distStatsTC[d].getMax();
    cout << endl;
  }
}

