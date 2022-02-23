
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
//  yyparse();
  if (argc < 6) {
    cout << argc << endl;
    usage();
  }
  int no = atoi(argv[1]);
  int ni = atoi(argv[2]);
  int ns = atoi(argv[3]);
  int nt = atoi(argv[4]);
  long numfsms = atol(argv[5]);
  int seed = atoi(argv[6]);
  char * stopFile = argv[7];
  srand(seed);
  BigNumber kSC = 0;
  BigNumber kTC = 0;
  BigNumber kRSC = 0;
  BigNumber numGenerated = 0;
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
//  graphTC.printWithoutPrefix("seq tc:");
  Graph graphRSC(fsm);
  graphRSC.spanReducedStateCounting();
  cout << "INFO>Total Length for RSC: " << graphRSC.totalLength() << endl;
//  graphRSC.printWithoutPrefix("seq rsc:");
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
    m1.generateRandomFull(ns, ni, no);
    m1.setup();
    BigNumber bnm1 = m1;
    if (generatedFSMs.count(bnm1) > 0) {
      j--;
      continue;
    } else {
      generatedFSMs.insert(bnm1);
      numGenerated++;
    }
    int dist;
    dist = fsm.outputDistance(m1); 
    //dist = fsm.shortestSequenceDistance(m1); 
    if (minDistance > dist) {
      minDistance = dist;
    }
    if (maxDistance < dist) {
      maxDistance = dist;
    }
    GraphNode * tckseq  = NULL;
    GraphNode * rsckseq = NULL;
    GraphNode * sckseq  = NULL;
    if (!fsm.isQuasiEq(m1)) {
      long eq;
      long df;
      tckseq = graphTC.compareOutput(fsm, m1, eq, df, true);
      if (df > 0) {
        distStatsTC[dist].first++;
        kTC++;
      } else {
        distStatsTC[dist].second++;
      }
      rsckseq = graphRSC.compareOutput(fsm, m1, eq, df, true);
      if (df > 0) {
        kRSC++;
        distStatsRSC[dist].first++;
      } else {
        distStatsRSC[dist].second++;
      }
      sckseq = graphSC.compareOutput(fsm, m1, eq, df, true);
      if (df > 0) {
        kSC++;
        distStatsSC[dist].first++;
      } else {
        distStatsSC[dist].second++;
      }
      numNQE++;
    } else {
      numQE++;
    }
    if (long(numGenerated % BigNumber(10000)) == 10000 - 1 || numGenerated == numToBeGenerated) {
      cout << ">>><HR>" << endl;
      cout << ">>>Number of Mutant FSMs: " << numGenerated << " of " << numToBeGenerated << " ("   <<  bnPercentage(numGenerated, numToBeGenerated) << "%)" << endl;
      cout << ">>>Number of Quasi EQ FSMs: " << numQE << endl;
      if (numNQE > BigNumber(0)) {
        cout << ">>>Number of Mutant FSMs killed by TC seqs: " << kTC << " " << numNQE << " (" << bnPercentage(kTC, numNQE) << "%)" << endl;
        cout << ">>>Number of Mutant FSMs killed by RSC seqs: " << kRSC << " " << numNQE << " (" << bnPercentage(kRSC, numNQE) << "%)" << endl;
        cout << ">>>Number of Mutant FSMs killed by SC seqs: " << kSC << " " << numNQE << " (" << bnPercentage(kSC, numNQE) << "%)" << endl;
      }
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
//  cout << "<TABLE>" << endl;
//  cout << "<TR></TR>" << endl;
  BigNumber totalNonKilledTC = 0;
  BigNumber totalNonKilledRSC = 0;
  if (minDistance == 0) {
    minDistance = 1;
  }
  for(int d = minDistance; d <= maxDistance; d++) {
    cout << ">>>Distance: " << d << endl;
    BigNumber numKilled;
    BigNumber numTotal;
    numKilled = (distStatsTC[d].first);
    numTotal = (distStatsTC[d].first + distStatsTC[d].second);
    totalNonKilledTC += (distStatsTC[d].second);
    cout << ">>> num of FSMs: " << numTotal << " (" << bnPercentage(numTotal, numNQE) << "%)" << endl;
    cout << ">>> num killed by TC: " << numKilled << " of " << numTotal << " (" << bnPercentage(numKilled, numTotal) << "%)" << endl;

    numKilled = (distStatsRSC[d].first);
    numTotal = (distStatsRSC[d].first + distStatsRSC[d].second);
    totalNonKilledRSC += (distStatsRSC[d].second);
    cout << ">>> num killed by RSC: " << numKilled << " of " << numTotal << " (" << bnPercentage(numKilled, numTotal) << "%)" << endl;

    numKilled = (distStatsSC[d].first);
    numTotal = (distStatsSC[d].first + distStatsSC[d].second);
    cout << ">>> num killed by SC: " << numKilled << " of " << numTotal << " (" << bnPercentage(numKilled, numTotal) << "%)" << endl;
  }
  BigNumber sumNonKilledTC = 0;
  BigNumber sumTotalTC = 0;
//  for(int d = (int) distStatistics.getMin(); d <= (int) distStatistics.getMax(); d++) {
  cout << "INFO>BEGIN DTC" << endl;
  for(int d = 1; d <= maxDistance; d++) {
    BigNumber numKilled;
    BigNumber numNonKilled;
    BigNumber numTotal;
    numKilled = (distStatsTC[d].first);
    numNonKilled = (distStatsTC[d].second);
    numTotal = numKilled + numNonKilled;
    sumTotalTC += numTotal;
    sumNonKilledTC += numNonKilled;
    cout << "DTC>" << d << " " << numTotal - numKilled << " " << sumNonKilledTC << " " << totalNonKilledTC << " " << numTotal << " " << sumTotalTC << " " << numGenerated - numQE << endl;
  }
  cout << "INFO>END DTC" << endl;
  BigNumber sumNonKilledRSC = 0;
  BigNumber sumTotalRSC = 0;
//  for(int d = (int) distStatistics.getMin(); d <= (int) distStatistics.getMax(); d++) {
  cout << "INFO>BEGIN DRSC" << endl;
  for(int d = 1; d <= maxDistance; d++) {
    BigNumber numKilled;
    BigNumber numNonKilled;
    BigNumber numTotal;
    numKilled = (distStatsRSC[d].first);
    numNonKilled = (distStatsRSC[d].second);
    numTotal = numKilled + numNonKilled;
    sumTotalRSC += numTotal;
    sumNonKilledRSC += numNonKilled;
    cout << "DRSC>" << d << " " << numTotal - numKilled << " " << sumNonKilledRSC << " " << totalNonKilledRSC << " " << numTotal << " " << sumTotalRSC << " " << numGenerated - numQE << endl;
  }
  cout << "INFO>END DRSC" << endl;
  cout << ">>><hr>\n";
  cout << ">>><b>Done!</b><br>\n";
}

