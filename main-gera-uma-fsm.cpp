
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
#include <algorithm>
#include <cstring>
//#include <stdlib.h> 

#include "BigNumber.h"
//#define MAXGER 100000

void yyparse(void);

/*char* itoa(int val, int base){
	
	static char buf[32] = {0};
	
	int i = 30;
	
	for(; val && i ; --i, val /= base)
	
		buf[i] = "0123456789abcdef"[val % base];
	
	return &buf[i+1];
	
}
*/

string itoa(int value, int base) {
	
	enum { kMaxDigits = 35 };
	
	std::string buf;
	
	buf.reserve( kMaxDigits ); // Pre-allocate enough space.
	
	// check that the base if valid
	
	if (base < 2 || base > 16) return buf;
	

	
	int quotient = value;
	

	
	// Translating number to string with base:
	
	do {
	
		buf += "0123456789abcdef"[ std::abs( quotient % base ) ];
	
		quotient /= base;
	
	} while ( quotient );
	

	
	// Append the negative sign for base 10
	
	if ( value < 0 && base == 10) buf += '-';
	

	
	std::reverse( buf.begin(), buf.end() );
	
	return buf;
	
}



void usage() {
  cout << "usage: sc numOuts numInps numStates numVariants seed\n";
  exit(1);
}
//Programa que gera 1 fsm randomicamente por parametros de entrada.
//Gera 1 fsm e salva no arquivo fornecido na entrada do programa.
//a fsm gerada eh reduzida.
//aplica o State Counting e gera o conjunto para a fsm criada. Ex: 1_T.in


int main(int argc, char * argv[]) {
//  yyparse();
	BigNumber fsmcreated = 0;
	BigNumber gerados = 0;	
	BigNumber nkTC = 0;
  int no = atoi(argv[1]);		//num de saidas
  int ni = atoi(argv[2]);		//num de entradas
  int ns = atoi(argv[3]);		//num de estados
  int porct = atoi(argv[4]);		//porcentagem de transicoes ex: '60' (gera 60% das transicoes possiveis)
//  long numfsms = atol(argv[5]);	//num de fsm a ser geradas
  int seed = atoi(argv[5]);
	string arquivoMef = argv[6];
	string arquivoSC = argv[7];
	
  srand(seed);
  set<BigNumber> generatedFSMs;
  //BigNumber numToBeGenerated = numfsms;

	//calcula o numero de transicoes
	int numt = (int)round(porct/100.0 * ((ns * (ni - 1) + 1)) + ns - 1);
    cout << "numt: " << numt << " (ns: " << ns << ")" << endl;
  
  int numRepSeq = 0;
  for (int f = 0; f < 1 && numRepSeq < 1*30; f++)
  {
	  //cria os arquivos para salvar as mefs na pasta MEFS/
	  /*string aa = "MEFS/";
	  aa.append(itoa(f+1,10));
	  aa.append(".in");
	  cout << "nome: " << aa << endl;
	  */
	  
	  cout << "nomeArquivoMEF: " << arquivoMef << endl;
	  char arq[256];
	  //strcpy( arq, aa.c_str() );
	  strcpy( arq, arquivoMef.c_str() );
	  char * fsmfile = arq;
	  ofstream fsmfs(fsmfile); 

	  
//criar arquivo com as seq. geradas pelo SC
	  /*string seq = "MEFS/";
	  seq.append(itoa(f+1,10));
	  seq.append("_T.in");
	  cout << "nome: " << seq << endl;
	  */
	  
	  cout << "nomeArquivoSC: " << arquivoSC << endl;
	  char arq2[256];
	  //strcpy( arq2, seq.c_str() );
	  strcpy( arq2, arquivoSC.c_str() );
	  char * Tfile = arq2;
	  ofstream Tfs(Tfile); 
	  
	  
	  
/* converter string para char

	char xar_pointer[256];
	string estringue = new String( "uma string" );
	strcpy( xar_pointer, estringue.c_str() );
*/

	  
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
//     cout << "FSM " << f + 1 << endl;
     fsm.setup();
     BigNumber bnm1 = fsm;
	 
     GraphNode::setNumChildren(fsm.numInputs);
     Graph graphSC(fsm);
     if (generatedFSMs.count(bnm1) > 0) {	//se a fsm gerada já tiver sido
       f--;									//gerada, entao cancela o laço
       numRepSeq++;							//e gera outra fsm randomicamente
cout << " ja gerou " << endl;
	   continue;
     } else {
       if (fsm.sameInputOutputPairs()) {	//se a fsm tiver 2 transicoes
         f--;								//iguais(repetidas), entao
         numRepSeq++;						//cancela o laço
cout << " transicoes iguais " << endl;
		   continue;							//e gera outra fsm randomicamente
       } else {
		 if (!fsm.isReduced()) {			//se a fsm nao for reduzida, entao
         f--;								//cancela o laço
         numRepSeq++;						//e gera outra fsm randomicamente
cout << " nao eh reduzida " << endl;         
			 continue;							
         } else {
             generatedFSMs.insert(bnm1);
             numGenerated++;
             numRepSeq = 0;
           }
         }
	   }

	 fsm.printOn(fsmfs,"");
	 
	 //aplicar o SC e salvar em arquivos
	 graphSC.spanStateCounting(fsm.getNumStates());
	 graphSC.saveOn(Tfs,"");
	 
	 
  }
}
