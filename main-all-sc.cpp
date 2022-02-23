
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

#include "BigNumber.h"

void yyparse(void);

FSM fsm;

void usage() {
  cout << "usage: sc numOuts numInps numStates numVariants seed\n";
  exit(1);
}

int main(int argc, char * argv[]) {
  yyparse();
  char * stopFile = argv[1];
  int distType = atoi(argv[2]);
  fsm.setup();
  BigNumber kSC = 0;
  BigNumber kTC = 0;
  BigNumber kRSC = 0;
  BigNumber numGenerated = 0;
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
  BigNumber numNQE = 0;
  BigNumber numQE = 0;
  int minDistance = 10000;
  int maxDistance = -10000;
  map<int,pair<BigNumber, BigNumber> > distStatsTC;
  map<int,pair<BigNumber, BigNumber> > distStatsRSC;
  map<int,pair<BigNumber, BigNumber> > distStatsSC;
  FSM m1(fsm);
  m1.initFSM();
  BigNumber numToBeGenerated = Power(BigNumber(fsm.getNumStates() * fsm.getNumOutputs()), fsm.getNumStates() * fsm.getNumInputs());
  cout << ">>>" << numToBeGenerated << " FSMs will be generated<BR>\n";
  while (m1.incFSM()) {
    numGenerated++;
    m1.setup();
    int dist;
    if (distType == OUTPUTDIST) {
      dist = fsm.outputDistance(m1); 
    } else if (distType == LENMINDIST) {
      dist = fsm.shortestSequenceDistance(m1);
    }
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
    cout << ">>><hr>" << endl;
    cout << ">>>Distance: " << d << endl;
    BigNumber numKilled;
    BigNumber numTotal;
    numKilled = (distStatsTC[d].first);
    numTotal = (distStatsTC[d].first + distStatsTC[d].second);
    totalNonKilledTC += (distStatsTC[d].second);
    cout << ">>> num of FSMs: " << numTotal << " (" << bnPercentage(numTotal, numNQE) << "% of all mutants)" << endl;
    cout << ">>> num killed by TC: " << numKilled << " of " << numTotal << " (" << bnPercentage(numKilled, numTotal) << "%)" << endl;
    cout << ">>> num not killed by TC: " << numTotal - numKilled << " of " << numNQE - kTC << " (" << bnPercentage(numTotal - numKilled, numNQE - kTC) << "% of all mutants not killed by TC)" << endl;

    numKilled = (distStatsRSC[d].first);
    numTotal = (distStatsRSC[d].first + distStatsRSC[d].second);
    totalNonKilledRSC += (distStatsRSC[d].second);
    cout << ">>> num killed by RSC: " << numKilled << " of " << numTotal << " (" << bnPercentage(numKilled, numTotal) << "%)" << endl;
    cout << ">>> num not killed by RSC: " << numTotal - numKilled << " of " << numNQE - kRSC << " (" << bnPercentage(numTotal - numKilled, numNQE - kRSC) << "% of all mutants not killed by RSC)" << endl;

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

