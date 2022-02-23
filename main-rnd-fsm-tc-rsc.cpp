
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
#define MAXSCLEN 500000

void yyparse(void);

FSM fsm;

void usage() {
  cout << "usage: sc numOuts numInps numStates numVariants seed\n";
  exit(1);
}

int main(int argc, char * argv[]) {
  yyparse();
  fsm.setup();
  fsm.debug(cout);
  BigNumber tcpegou = 0;
  
//  long numfsms = atol(argv[5]);
  long numMuts = atol(argv[1]);
  int seed = atoi(argv[2]);
  char * stopFile = argv[3];
  char * tmpfile = argv[4];
  char * resfile = argv[5];
  char * fsmfile = argv[6];	
  srand(seed);
  set<BigNumber> generatedFSMs;
/*  BigNumber numToBeGenerated = numfsms;
  BigNumber maxNumToBeGenerated = Power(BigNumber(ns * no), (ns * ni));
  if (numToBeGenerated > maxNumToBeGenerated) {
    numToBeGenerated = maxNumToBeGenerated;
  }
  numfsms = numToBeGenerated;
*/
  ofstream resofs(resfile);
  ofstream fsmnkRSC(fsmfile);		//adicionada
  fsmnkRSC << "FSM que RSC nao pegou" << endl;
  int numRepSeq = 0;
//  for (int f = 0; f < 1 && numRepSeq < numfsms*3; f++) {
    BigNumber kSC = 0;
    BigNumber kTC = 0;
    BigNumber kRSC = 0;
    BigNumber numGenerated = 0;
/*    FSM fsm;
    fsm.generateRandom(ns, ni, no);
     for (int i = ns - 1; i < nt; i++) {
      fsm.addRandomTransition();
     }
     fsm.setup();
*/
     BigNumber bnm1 = fsm;
//     cerr << f << " " << bnm1 << " " << numRepSeq << endl;
     GraphNode::setNumChildren(fsm.numInputs);
     Graph graphSC(fsm);
/*     if (generatedFSMs.count(bnm1) > 0) {	//se a fsm gerada já tiver sido
       f--;									//gerada, entao cancela o laço
       numRepSeq++;							//e gera outra fsm randomicamente
       continue;
     } else {
       if (fsm.sameInputOutputPairs()) {	//se a fsm tiver 2 transicoes
         f--;								//iguais(repetidas), entao
         numRepSeq++;						//cancela o laço
         continue;							//e gera outra fsm randomicamente
       } else {
         graphSC.spanStateCounting(fsm.getNumStates());
         if (graphSC.totalLength() > MAXSCLEN) {	//se o tam da seq. for
           f--;										//maior que 500.000, entao
           numRepSeq++;								//cancela o laço
           continue;						//e gera outra fsm randomicamente
         } else {
           generatedFSMs.insert(bnm1);
           numGenerated++;
           numRepSeq = 0;
         }
       }
     }
*/     ofstream tmpofs(tmpfile);
//     tmpofs << ">>>FSM " << 1 << " of " << 1 << endl;
     fsm.printOn(tmpofs, "FSM>");
     Graph graphTC(fsm);
     graphTC.spanTransitionCover();
     tmpofs << ">>>" << "Total Length for TC: " << graphTC.totalLength() << endl;
     Graph graphRSC(fsm);
     graphRSC.spanReducedStateCounting();
     tmpofs << ">>>" << "Total Length for RSC: " << graphRSC.totalLength() << endl;
//     Graph graphSC(fsm);
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
    set<BigNumber> generatedMuts;
    int numRepMutSeq = 0;
	//o for termina quando encontrar o numero 'numMuts' que nao sao pegos pelo TC
    for (int j = 0; j < numMuts && numRepMutSeq < numMuts * 3; j++) {
      FSM m1;
      m1.generateRandomFull(fsm.getNumStates(), fsm.intToState, fsm.getNumInputs(), fsm.intToInput, fsm.getNumOutputs(), fsm.intToOutput);
      m1.setup();
      BigNumber bnm1 = m1;
      if (generatedMuts.count(bnm1) > 0) { //cancela se a fsm mutante ja foi gerada anteriormente
       j--;
       numRepMutSeq++;
       tmpofs << bnm1 << endl;
       continue;
      } else {
       generatedMuts.insert(bnm1);
       numRepMutSeq = 0;
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
      if (!fsm.isQuasiEq(m1)) {		//calcula a seq. se as fsms nao forem
        long eq;					//quasi-equivalente
        long df;
        tckseq = graphTC.compareOutput(fsm, m1, eq, df, true);
        if (df > 0) {							//se TC matou entao 
		  j--;									//seleciona outro mutante
		  tcpegou++;	//TC pegou		
		  continue;
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
			fsmnkRSC << bnm1 << endl;
        }
        //sckseq = graphSC.compareOutput(fsm, m1, eq, df, true);
        df = 1;
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
//      if (long(numGenerated % BigNumber(10000)) == 10000 - 1 || numGenerated == numToBeGenerated) {
        tmpofs << ">>><HR>" << endl;
//        tmpofs << ">>>Number of Mutant FSMs: " << 1 << " of " << 1 << " ("   <<  bnPercentage(numGenerated, numToBeGenerated) << "%)" << endl;
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
      //}
    }
	tmpofs << ">>>TC tinha pegado: " << tcpegou << endl;
    tmpofs.close();
    resofs << "<FSM>" << endl;
    resofs << bnm1 << endl;
    resofs << 0 << " " << numQE << " " << numNQE << " " << kTC << " " << kRSC << " " << kSC << endl;
    resofs << "<TC>" << graphTC.totalLength() << "</TC>" << endl;
    resofs << "<RSC>" << graphRSC.totalLength() << "</RSC>" << endl;
    resofs << "<SC>" << graphSC.totalLength() << "</SC>" << endl;

    for(int d = minDistance; d <= maxDistance; d++) {
      resofs << d << " ";
      resofs << (distStatsTC[d].first)  << " " << (distStatsTC[d].first + distStatsTC[d].second) << " ";
      resofs << (distStatsRSC[d].first) << " " << (distStatsRSC[d].first + distStatsRSC[d].second) << " ";
      resofs << (distStatsSC[d].first)  << " " << (distStatsSC[d].first + distStatsSC[d].second);
      resofs << endl;
    }
    resofs << "</FSM>" << endl;
//  }
}
