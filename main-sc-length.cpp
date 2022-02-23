#include <iostream>
#include <sstream>
#include "BigNumber.h"
#include "FSM.h"
#include "Graph.h"

using namespace std;

int main(int argc, char * argv[]) {
//  yyparse();
  int no = atoi(argv[1]);
  int ni = atoi(argv[2]);
  int ns = atoi(argv[3]);
  BigNumber bn;
  while (cin >> bn) {
    FSM fsm;
    fsm.readFromBigNumber(ns, ni, no, bn);
    GraphNode::setNumChildren(ni);
    Graph graphTC(fsm);
    graphTC.spanTransitionCover();
    cout << "<TC>" << graphTC.totalLength() << "</TC>" << endl;
    Graph graphRSC(fsm);
    graphRSC.spanReducedStateCounting();
    cout << "<RSC>" << graphRSC.totalLength() << "</RSC>" << endl;
    Graph graphSC(fsm);
    graphSC.spanStateCounting(ns);
    cout << "<SC>" << graphSC.totalLength() << "</SC>" << endl;
  }
}
