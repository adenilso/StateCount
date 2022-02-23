#ifndef __GRAPH_H__
#define __GRAPH_H__

#include "FSM.h"

struct GraphNode {
  GraphNode(GraphNode *par, int pi, int s);
  GraphNode *parent;
  int parentInput;
  int state;
  GraphNode **children;
  ~GraphNode();
  static void setNumChildren(int numC);
  private:
  static int numChildren;
};

class Graph {
  public:
    Graph(FSM &fsm);
    GraphNode * setRoot(int state); 
    GraphNode * getRoot(int state) {
      return parent;
    }
    GraphNode * addNode(GraphNode *, int pi, int state); 
    GraphNode * addNode(GraphNode *, const vector<int> &);
    GraphNode * addNode(GraphNode *n, int pi) {
      if (n) {
        return addNode(n, pi, fsm.nextState[fsm.calcPos(n -> state, pi)]);
      } else {
        return NULL;
      }
    }

    void saveOn(ostream &, const string &prefix = "");//metodo adicionado para salvar seq. do SC
    void calcLongestChain(GraphNode *, GraphNode *, int, vector<GraphNode *> &);
    void spanStateCounting(int);
    void spanTransitionCover();
    void spanReducedStateCounting();
    void spanReducedStateCountingK(int); //metodo adicionado
    void spanReducedStateCountingK_AUX(int);  //metodo adicionado
    void spanSingleTransitionCover();
    long int totalLength();
    long int totalLengthWithoutPrefix();
    void printWithoutPrefix(const string &);
    void printSeqFromLast(const string &, GraphNode *last);
    void printSeqFromLastWithStates(const string &, GraphNode *last);
    void collectLeafes(bool cached = true);
    void getInputSeq(GraphNode *last, vector<int> &);
    void getInputSeq(GraphNode *last, vector<GraphNode *> &);
    void getInputSeq(GraphNode *last, vector<string> &);
    GraphNode * compareOutput(FSM &m1, FSM &m2, long &ne, long &nd, bool onlyFirst = false);
    ~Graph();

  protected:
    vector<GraphNode *> leafes;
    GraphNode * parent;
  private:
     FSM &fsm;
    void printWithoutPrefixAux(const string &, GraphNode *);
    void calcLongestChainAux(GraphNode *, GraphNode *, int, vector<GraphNode *> &);
    void spanStateCountingAux(int, GraphNode *, GraphNode *);
    long int totalLengthAux(GraphNode *, int depth);
    long int totalLengthWithoutPrefixAux(GraphNode *, int depth);
    void collectLeafesAux(GraphNode *);
};

void spanUpToNthLevelAux(const FSM &fsm, Graph *graph, GraphNode * node, int level, int maxLevel);

Graph * spanUpToNthLevel(const FSM &fsm, int maxLevel);

#endif
