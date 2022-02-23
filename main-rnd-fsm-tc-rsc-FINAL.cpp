
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
//#define MAXGER 100000

void yyparse(void);

void usage() {
  cout << "usage: sc numOuts numInps numStates numVariants seed\n";
  exit(1);
}
//Programa que gera fsm randomicamente por parametros de entrada.
//Gera diversas fsms diferentes para a fsm criada. Aplica o RSC para
//as fsm que o TC nao matou.


int main(int argc, char * argv[]) {
//  yyparse();
	BigNumber fsmcreated = 0;
	BigNumber gerados = 0;	
	BigNumber nkTC = 0;
  int no = atoi(argv[1]);		//num de saidas
  int ni = atoi(argv[2]);		//num de entradas
  int ns = atoi(argv[3]);		//num de estados
  int porct = atoi(argv[4]);		//porcentagem de transicoes ex: '60' (gera 60% das transicoes possiveis)
  long numfsms = atol(argv[5]);	//num de fsm a ser geradas
  long numMuts = atol(argv[6]); //num de mutantes gerado para cada fsm 
  int seed = atoi(argv[7]);
  char * stopFile = argv[8];
  char * tmpfile = argv[9];
  //char * resfile = argv[10];  //arq. que grava as fsm na notacao para ser utilizadas no gnuplot
  char * plotfile = argv[10];	
  char * fsmfile = argv[11];	//arq. que grava as 100 fsm mutantes nao matadas pelo TC para cada FSM
  srand(seed);
  set<BigNumber> generatedFSMs;
  BigNumber numToBeGenerated = numfsms;

	//calcula o numero de transicoes
	int numt = (int)round(porct/100.0 * ((ns * (ni - 1) + 1)) + ns - 1);
    cout << "numt: " << numt << " (ns: " << ns << ")" << endl;

  ofstream tmpofs(tmpfile);
  ofstream plotfs(plotfile);
  ofstream fsmfs(fsmfile);
  
  plotfs << no << " " << ni << " " << ns<< " " << porct << " " << numfsms << " " << numMuts << endl;
  
  int numRepSeq = 0;
  for (int f = 0; f < numfsms && numRepSeq < numfsms*3; f++) {
	 // cout << "for: "<< f << endl;
	  nkTC = 0;
	 fsmcreated = 0;
	 gerados = 0;
    BigNumber kTC = 0;
    BigNumber kRSC = 0;
    BigNumber numGenerated = 0;
    FSM fsm;
    fsm.generateRandom(ns, ni, no);
     for (int i = ns - 1; i < numt; i++) {	//adiciona as trans. restantes, pois
      fsm.addRandomTransition();			//a fsm ja eh inicializada com as trans.
	 }								 		//do estado inicial para os demais.
     cout << "FSM " << f + 1 << endl;
     fsm.setup();
     BigNumber bnm1 = fsm;
	 
	 //cout << ">>>MEF<<<< " << endl;
     //fsm.printOn(cout, "FSM>");
     //fsm.printDotOn(cout, "DOT>");
     GraphNode::setNumChildren(fsm.numInputs);
     Graph graphSC(fsm);
     if (generatedFSMs.count(bnm1) > 0) {	//se a fsm gerada já tiver sido
       f--;									//gerada, entao cancela o laço
       numRepSeq++;							//e gera outra fsm randomicamente
       continue;
     } else {
       if (fsm.sameInputOutputPairs()) {	//se a fsm tiver 2 transicoes
         f--;								//iguais(repetidas), entao
         numRepSeq++;						//cancela o laço
         continue;							//e gera outra fsm randomicamente
       } else {
           generatedFSMs.insert(bnm1);
           numGenerated++;
           numRepSeq = 0;
         }
     }
     //ofstream tmpofs(tmpfile);
     tmpofs << ">>>FSM " << f + 1 << " of " << numfsms << endl;
	 fsm.printOn(tmpofs, "FSM>");
     Graph graphTC(fsm);
     graphTC.spanTransitionCover();
     tmpofs << ">>>" << "Total Length for TC: " << graphTC.totalLength() << endl;
     Graph graphRSC(fsm);
     graphRSC.spanReducedStateCounting();
     tmpofs << ">>>" << "Total Length for RSC: " << graphRSC.totalLength() << endl;
     tmpofs << "INFO>END FSM" << endl;
	 fsmfs << "FSM " << f + 1 << " MUTANTS:" << endl; //Arquivo de mutantes
	 
    BigNumber numNQE = 0;
    BigNumber numQE = 0;
    set<BigNumber> generatedMuts;
    int numRepMutSeq = 0;
	long numMutsAux = numMuts;
	//o for termina quando encontrar o numero 'numMuts' que nao sao pegos pelo TC
    for (int j = 0; j < numMutsAux && numRepMutSeq < numMuts * 3; j++) {
      FSM m1;
		
      m1.generateRandomFull(fsm.getNumStates(), fsm.intToState, fsm.getNumInputs(), fsm.intToInput, fsm.getNumOutputs(), fsm.intToOutput);
      m1.setup();
	  BigNumber bnMut = m1;	
      GraphNode * tckseq  = NULL;
      GraphNode * rsckseq = NULL;
      if (!fsm.isQuasiEq(m1)) {		//calcula a seq. se as fsms nao forem
                long eq;					//quasi-equivalente
                long df;
	        gerados++;		//variavel que vai de 0 a 100000 para verificar se há algum nKTC entre esses 100000.       
            fsmcreated++;		//variavel que contabiliza as fsm criadas

	        tckseq = graphTC.compareOutput(fsm, m1, eq, df, true);
                if (df > 0) {							//se TC matou entao 
	        	j--;									//seleciona outro mutante
                } else {
                        gerados = 0;
	        			nkTC++;
                        rsckseq = graphRSC.compareOutput(fsm, m1, eq, df, true);
                        if (df > 0) {
                                kRSC++;
                        }
                        // TODO: Imprimir o bignumber de m1
						fsmfs <<  j + 1<<": "<< bnMut << endl;		//grava o mutante
                numNQE++;
				  }
                
      } else {
                numQE++;
		j--;  
      }

      if( j == numMutsAux - 1) {
          	plotfs << f + 1 << " " << bnPercentage(kTC, numNQE) << " " << bnPercentage(kRSC, numNQE) << " " << numQE << " " ;
          	if (numNQE > BigNumber(0)) {
                        tmpofs << ">>>Number of Mutant FSMs killed by TC seqs: " << kTC << " " << numNQE << " (" << bnPercentage(kTC, numNQE) << "%)" << endl;
                        tmpofs << ">>>Number of Mutant FSMs killed by RSC seqs: " << kRSC << " " << numNQE << " (" << bnPercentage(kRSC, numNQE) << "%)" << endl;
          		tmpofs << ">>>Number of Quasi EQ FSMs: " << numQE << endl;
                }	
      }
	  
      if (gerados > BigNumber(100000))
      {  cout << "entrou if gerados: " << gerados << endl;
      	        if(nkTC > BigNumber(0))
      	        {
      	        	numMutsAux--; //variavel que reduz o numero de mutantes a ser gerados para determinada FSM.
      	        	if (BigNumber(numMutsAux) == BigNumber(nkTC)) //se for true nao entra mais no laco e eh preciso imprimir
					{
						tmpofs << ">>>Number of Mutant FSMs killed by TC seqs: " << kTC << " " << numNQE << " (" << bnPercentage(kTC, numNQE) << "%)" << endl;
                    	tmpofs << ">>>Number of Mutant FSMs killed by RSC seqs: " << kRSC << " " << numNQE << " (" << bnPercentage(kRSC, numNQE) << "%)" << endl;
          				tmpofs << ">>>Number of Quasi EQ FSMs: " << numQE << endl;
					}					
					cout << "reduzindo: numMutsAux = " << numMutsAux << " nKTC: " << nkTC << endl; 
      	        }
      	        else	//aborta a FSM e escolhe outra
      	        {
      	        	tmpofs << " nkTC: " << nkTC << " fsmCreated: "<< fsmcreated<< " Escolhendo outra FSM "<< endl;
      	        	f--;			
                    break;
      	        }	
      	        gerados = 0;
      }
			
/*
# se numero de gerados > MAXGER entao
#   se numero de nao mortos pelo TC > 0 entao
#     decrementar numero de TC total
#   senao
#     abortar a mef e procurar outra
#   fim se
#   zerar contador de gerados
# fim se		
*/		
    }
    tmpofs << ">>>Number of FSM created: " << fsmcreated << endl;
    plotfs << fsmcreated << endl;
    tmpofs << " " << endl;
    //tmpofs.close();
  }
}
