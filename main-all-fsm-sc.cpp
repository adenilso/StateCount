
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

void usage() {
  cout << "usage: sc numOuts numInps numStates numVariants seed\n";
  exit(1);
}

int main(int argc, char * argv[]) {
//  yyparse();
  int no = atoi(argv[1]);
  int ni = atoi(argv[2]);
  int ns = atoi(argv[3]);
  int nt = atoi(argv[4]);
  long numfsms = atol(argv[5]);
  int seed = atoi(argv[6]);
  char * stopFile = argv[7];
  char * tmpfile = argv[8];
  char * resfile = argv[9];
  srand(seed);
  set<BigNumber> generatedFSMs;
  BigNumber numToBeGenerated = numfsms;
  BigNumber maxNumToBeGenerated = Power(BigNumber(ns * no), (ns * ni));
  if (numToBeGenerated > maxNumToBeGenerated) {
    numToBeGenerated = maxNumToBeGenerated;
  }
  numfsms = numToBeGenerated;
  ofstream resofs(resfile);
  int numRepSeq = 0;
  for (int f = 0; f < numfsms && numRepSeq < numfsms*10; f++) {
    BigNumber kSC = 0;
    BigNumber kTC = 0;
    BigNumber kRSC = 0;
    BigNumber numGenerated = 0;
    FSM fsm;
    fsm.generateRandom(ns, ni, no);
     for (int i = ns - 1; i < nt; i++) {
      fsm.addRandomTransition();
     }
     fsm.setup();
     BigNumber bnm1 = fsm;
//     cerr << f << " " << bnm1 << " " << numRepSeq << endl;
     if (generatedFSMs.count(bnm1) > 0) {
       f--;
       numRepSeq++;
       continue;
     } else {
       generatedFSMs.insert(bnm1);
       numGenerated++;
       numRepSeq = 0;
     }
     GraphNode::setNumChildren(fsm.numInputs);
     ofstream tmpofs(tmpfile);
     tmpofs << ">>>FSM " << f + 1 << " of " << numfsms << endl;
     fsm.printOn(tmpofs, "FSM>");
     Graph graphTC(fsm);
     graphTC.spanTransitionCover();
     tmpofs << ">>>" << "Total Length for TC: " << graphTC.totalLength() << endl;
     Graph graphRSC(fsm);
     graphRSC.spanReducedStateCounting();
     tmpofs << ">>>" << "Total Length for RSC: " << graphRSC.totalLength() << endl;
     Graph graphSC(fsm);
     graphSC.spanStateCounting(fsm.getNumStates());
     tmpofs << ">>>" << "Total Length for SC: " << graphSC.totalLength() << endl;
    tmpofs << "INFO>END FSM" << endl;
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
    tmpofs << ">>>" << numToBeGenerated << " FSMs will be generated<BR>\n";
    while (m1.incFSM()) {
      numGenerated++;
      m1.setup();
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
        tmpofs << ">>><HR>" << endl;
        tmpofs << ">>>Number of Mutant FSMs: " << numGenerated << " of " << numToBeGenerated << " ("   <<  bnPercentage(numGenerated, numToBeGenerated) << "%)" << endl;
        tmpofs << ">>>Number of Quasi EQ FSMs: " << numQE << endl;
        if (numNQE > BigNumber(0)) {
          tmpofs << ">>>Number of Mutant FSMs killed by TC seqs: " << kTC << " " << numNQE << " (" << bnPercentage(kTC, numNQE) << "%)" << endl;
          tmpofs << ">>>Number of Mutant FSMs killed by RSC seqs: " << kRSC << " " << numNQE << " (" << bnPercentage(kRSC, numNQE) << "%)" << endl;
          tmpofs << ">>>Number of Mutant FSMs killed by SC seqs: " << kSC << " " << numNQE << " (" << bnPercentage(kSC, numNQE) << "%)" << endl;
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
    tmpofs.close();
    resofs << "<FSM>" << endl;
    resofs << bnm1 << endl;
    resofs << f << " " << numQE << " " << numNQE << " " << kTC << " " << kRSC << " " << kSC << endl;

    for(int d = minDistance; d <= maxDistance; d++) {
      resofs << d << " ";
      resofs << (distStatsTC[d].first)  << " " << (distStatsTC[d].first + distStatsTC[d].second) << " ";
      resofs << (distStatsRSC[d].first) << " " << (distStatsRSC[d].first + distStatsRSC[d].second) << " ";
      resofs << (distStatsSC[d].first)  << " " << (distStatsSC[d].first + distStatsSC[d].second);
      resofs << endl;
    }
    resofs << "</FSM>" << endl;
  }
}

