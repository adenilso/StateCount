//codigo semelhante ao main-sc, mas produz o conjunto gerado pelo
//State Couting pronto para ser salvo em um arquivo de saida.
//O state counting eh aplicado para uma mef de entrada.
#include "FSM.h"
#include "Graph.h"
#include <set>
#include <iterator>
#include <iostream>
#include <fstream>

void yyparse(void);

FSM fsm;

void usage() {
  cout << "usage: fsm-sc-arq < mefEntrada.in arqSaidaConjStateCounting.txt \n";
  exit(1);
}

int main(int argc, char * argv[]) {
  yyparse();
  fsm.setup();
//  fsm.debug(cout);
  GraphNode::setNumChildren(fsm.getNumStates());
  Graph graphSC(fsm);
  int m = fsm.getNumStates();
  string arquivoSC = argv[1];
  //if (argc > 1) {
  //  m = atoi(argv[1]);
  //}
  
  	  cout << "nomeArquivoSC: " << arquivoSC << endl;
	  char arq2[256];
	  //strcpy( arq2, seq.c_str() );
	  strcpy( arq2, arquivoSC.c_str() );
	  char * Tfile = arq2;
	  ofstream Tfs(Tfile); 
	
//  cout << ">>>" << "Number of States in Implementation FSMs (m): " << m << endl;
  graphSC.spanStateCounting(m);
  graphSC.saveOn(Tfs,"");
//  graphSC.printWithoutPrefix("");
//  cout << ">>>" << "Total Length for SC: " << graphSC.totalLength() << endl;
}
