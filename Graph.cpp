
#include "Graph.h"
#include "FSM.h"
#include <stdlib.h>
#include <iostream>
#include <iterator>
#include <queue>

//#define DEBUG1
//#define VERBOSE				//adicionado Lucio
int GraphNode::numChildren;

void GraphNode::setNumChildren(int numC) {
  numChildren = numC;
}

GraphNode::GraphNode(GraphNode *par, int pi, int s) {
  parent = par;
  parentInput = pi;
  state = s;
  children = new GraphNode*[numChildren];
  for (int i = 0; i < numChildren; i++) {
    children[i] = NULL;
  }
}

GraphNode::~GraphNode() {
  if (children) {
    for (int i = 0; i < numChildren; i++) {
      if (children[i]) {
        delete children[i];
      }
    }
    delete [] children;
  }
}

Graph::Graph(FSM &m) : fsm(m) {
  parent = NULL;
  leafes.clear();
}

Graph::~Graph() {
  if (parent) {
    delete parent;
  }
}

GraphNode * Graph::setRoot(int state) {
  if (parent) {
    delete parent;
  }
  parent = new GraphNode(NULL, -1, state);
  return parent;
}

GraphNode * Graph::addNode(GraphNode *par, int pi, int s) {
  if (par -> children[pi] == NULL) {
    par -> children[pi] = new GraphNode(par, pi, s);
  }
  return par -> children[pi];
}

GraphNode * Graph::addNode(GraphNode *par, const vector<int> &seq) {
  GraphNode * state = par;
  for (vector<int>::const_iterator it = seq.begin(); it != seq.end(); it++) {
    state = addNode(state, *it);
  }
  return state;
}

void Graph::printWithoutPrefix(const string &pre) {
  collectLeafes();
  for (vector<GraphNode *>::const_iterator it = leafes.begin(); it != leafes.end(); it++) {
    printSeqFromLast(pre, *it);
    cout << endl;
  }
}

void Graph::collectLeafes(bool cached) {
  if (! cached || leafes.size() == 0) {
    leafes.clear();
    collectLeafesAux(parent);
  }
}

void Graph::collectLeafesAux(GraphNode *s) {
  if (s != NULL) {
    int numChildren = 0;
    for (int i = 0; i < fsm.numInputs; i++) {
      if (s -> children[i] != NULL) {
        collectLeafesAux(s -> children[i]);
        numChildren++;
      }
    }
    if (numChildren == 0) {
      leafes.insert(leafes.end(), s);
    }
  }
}

void Graph::printSeqFromLast(const string &pre, GraphNode *s) {
  cout << pre;
  vector<string> in;
  getInputSeq(s, in);
  for (vector<string>::const_iterator it = in.begin(); it != in.end(); it++) {
    cout << " " << *it;
  }
}

void Graph::printSeqFromLastWithStates(const string &pre, GraphNode *s) {
  cout << pre;
  vector<GraphNode *> in;
  getInputSeq(s, in);
  for (vector<GraphNode *>::const_iterator it = in.begin(); it != in.end(); it++) {
    GraphNode *gn = *it;
    if (gn -> parentInput != -1) {
      cout << " " << fsm.intToInput[gn -> parentInput];
    }
    cout << " (" << fsm.intToState[gn -> state] << ")";
  }
}

void Graph::getInputSeq(GraphNode *s, vector<int> &in) {
  if (s != NULL && s -> parentInput != -1) {
    getInputSeq(s -> parent, in);
    in.insert(in.end(), s -> parentInput);
  }
}

void Graph::getInputSeq(GraphNode *s, vector<GraphNode *> &in) {
  if (s != NULL && s -> parentInput != -1) {
    getInputSeq(s -> parent, in);
  }
  if (s != NULL) {
    in.insert(in.end(), s);
  }
}

void Graph::getInputSeq(GraphNode *s, vector<string> &in) {
  vector<int> iin;
  getInputSeq(s, iin);
  for (vector<int>::const_iterator it = iin.begin(); it != iin.end(); it++) {
    map<int, string>::const_iterator pair = fsm.intToInput.find(*it);
    in.insert(in.end(), (*pair).second);
  }
}

void Graph::calcLongestChain(GraphNode *from, GraphNode *to, int state, vector<GraphNode *> &chain) {
  while(from != to && from -> state != state) {
    from = from -> parent;
  }
  calcLongestChainAux(from, to, state, chain);
}

void Graph::calcLongestChainAux(GraphNode *from, GraphNode *to, int state, vector<GraphNode *> &chain) {
  if (from == to) {
    return;
  } else {
    calcLongestChainAux(from -> parent, to, state, chain);
    if (from -> state == state) {
      chain.insert(chain.end(), from);
    //} else if (fsm.isQE(from -> state, state)) {
    //} else if (fsm.isQE(state, from -> state)) {
    } else if (fsm.isQE(from -> state, state)) {
      vector<GraphNode *> p2;
      calcLongestChainAux(from -> parent, to, from -> state, p2);
      p2.insert(p2.end(), from);
      if (chain.size() < p2.size()) {
        chain.clear();
        copy(p2.begin(), p2.end(), insert_iterator<vector<GraphNode *> >(chain, chain.end()));
      }
    }
  }
}

void Graph::spanSingleTransitionCover() {
  setRoot(fsm.initialState);
  map<pair<int, int>, vector<int> > transferSeq;
  map<int, set<int> > uncovered;
  int numUncovered = 0;
  for (int i = 0; i < fsm.numStates; i++) {
    for (int j = 0; j < fsm.numInputs; j++) {
      int sl = fsm.nextState[fsm.calcPos(i, j)];
      if (sl > -1) {
        uncovered[i].insert(j);
        numUncovered ++;
        if (i != sl) {
          transferSeq[make_pair(i, sl)].push_back(j);
        }
      }
    }
  }
  int numChanges;
  do {
    numChanges = 0;
    for (int i1 = 0; i1 < fsm.numStates; i1++) {
      for (int j1 = 0; j1 < fsm.numStates; j1++) {
        if (i1 != j1) {
          const pair<int, int> &p1 = make_pair(i1, j1);
          if (transferSeq[p1].size() == 0) {
            int cst = -1;
            int ck = -1;
            unsigned cs = 0;
            for (int k2 = 0; k2 < fsm.numInputs; k2++) {
              int sl = fsm.nextState[fsm.calcPos(i1, k2)];
              if (sl > -1) {
                const pair<int, int> &p2 = make_pair(sl, j1);
                if (transferSeq[p2].size() > cs) {
                  cs = transferSeq[p2].size();
                  ck = k2;
                  cst = sl;
                }
              }
            }
            if (ck > -1) {
              const pair<int, int> &p2 = make_pair(cst, j1);
              transferSeq[p1].push_back(ck);
              copy(transferSeq[p2].begin(), transferSeq[p2].end(), insert_iterator<vector<int> >(transferSeq[p1], transferSeq[p1].end()));
              numChanges++;
            }
          }
        }
      }
    }
  } while (numChanges > 0);
  map<int, int> reachable;
  for (int i = 0; i < fsm.numStates; i++) {
    reachable[i] = 0;
    for (int j = 0; j < fsm.numStates; j++) {
      const pair<int, int> &p = make_pair(i, j);
      if (transferSeq[p].size() > 0) {
        reachable[i]++;
      }
    }
  }
  int s = fsm.initialState;
  vector<vector<int> > seqs;
  vector<int> seq;
  while (1) {
    if (uncovered[s].size() > 0) {
      int in = *uncovered[s].begin();
      uncovered[s].erase(in);
      seq.push_back(in);
      s = fsm.nextState[fsm.calcPos(s, in)];
      numUncovered--;
    } else {
      bool found = false;
      int ns = -1;
      for (int i = 0; i < fsm.numStates && !found; i++) {
        const pair<int, int> &p = make_pair(s, i);
        if (uncovered[i].size() > 0) {
          ns = i;
          if (transferSeq[p].size() > 0) {
            copy(transferSeq[p].begin(), transferSeq[p].end(), insert_iterator<vector<int> >(seq, seq.end()));
            s = i;
            found = true;
          }
        }
      }
      if (!found) {
        if (ns > -1) {
          addNode(parent, seq);
          seq.clear();
          const pair<int, int> &p = make_pair(fsm.initialState, ns);
          copy(transferSeq[p].begin(), transferSeq[p].end(), insert_iterator<vector<int> >(seq, seq.end()));
          s = ns;
        } else {
          break;
        }
      }
    }
  }
  addNode(parent, seq);
//  for (vector<vector<int> >::const_iterator it = seqs.begin(); it != seqs.end(); it++) {
//    const vector<int> & seq = *it;
//    cout << "1 seq: ";
//    copy(seq.begin(), seq.end(), ostream_iterator<int>(cout, " "));
//    cout << endl;
//  }
//
//  for (int i = 0; i < fsm.numStates; i++) {
//    for (int j = 0; j < fsm.numStates; j++) {
//      cout << fsm.intToState[i] << " --> " << fsm.intToState[j] << ": ";
//      const pair<int, int> &p = make_pair(i, j);
//      copy(transferSeq[p].begin(), transferSeq[p].end(), ostream_iterator<int>(cout, " "));
//      cout << endl;
//    }
//  }
}

void Graph::spanStateCounting(int m) {
  setRoot(fsm.initialState);
  for (set<vector<int> >::const_iterator it = fsm.coreCover.begin(); it != fsm.coreCover.end(); it++) {
    const vector<int> &coreSeq = *it;
//    cout << "alpha = ";
//    copy(coreSeq.begin(), coreSeq.end(), ostream_iterator<int>(cout, " "));
//    cout << endl;
    GraphNode * state = addNode(parent, coreSeq);
    spanStateCountingAux(m, state, state);
  }
}

void Graph::spanStateCountingAux(int m, GraphNode *from, GraphNode *state) {
#ifdef VERBOSE
  printSeqFromLast(">>>alpha: [", from);
  cout << "]" << endl;
  printSeqFromLastWithStates(">>> Sequence:", state);
  cout << endl;
#endif
  vector<GraphNode *> * chains = new vector<GraphNode *>[fsm.numStates];
  for (int i = 0; i < fsm.numStates; i++) {
    calcLongestChain(state, from, i, chains[i]);
#ifdef VERBOSE
  cout << ">>>longest chain for state " << fsm.intToState[i] << " = " << chains[i].size() << endl;
#endif
  }
  bool found = false;
  const set<int> * Rbeta = NULL;
  for (set<set<int> >::const_iterator it = fsm.R.begin(); it != fsm.R.end() && !found; it++) {
    unsigned int sum = 0;
    Rbeta = &(*it);
    for (set<int>::const_iterator itRB = Rbeta->begin(); itRB != Rbeta->end() && !found; itRB++) {
      int s = *itRB;
      sum += chains[s].size();
    }
    if (sum >= m - Rbeta->size() + 1) {
      found = true;
    }
  }
  if (! found) {
    int s = state -> state;
    for (int i = 0; i < fsm.numInputs; i++) {
      int nextState = fsm.nextState[fsm.calcPos(s,i)];
      if (nextState > -1) {
        GraphNode * newNode = addNode(state, i, nextState);
        spanStateCountingAux(m, from, newNode);
      }
    }
  } else {
#ifdef VERBOSE
  cout << ">>><B> Stop with Rbeta = {";
  for (set<int>::const_iterator itRB = Rbeta->begin(); itRB != Rbeta->end(); itRB++) {
    cout << " " << fsm.intToState[*itRB];
  }
  cout << "}</B>" << endl;
#endif
    set<int> Kbeta;
    for (set<int>::const_iterator itRB = Rbeta->begin(); itRB != Rbeta->end(); itRB++) {
      set<int> Sbeta = *Rbeta;
      int s = *itRB;
      for (vector<GraphNode *>::const_iterator it = chains[s].begin(); it != chains[s].end(); it++) {
        Sbeta.insert((*it)->state);
      }
      for (set<int>::const_iterator it = fsm.coreStates.begin(); it != fsm.coreStates.end(); it++) {
        bool useful = false;
        for (int i = 0; i < fsm.numStates; i++) {
          if (Sbeta.count(i) > 0 && fsm.isQE(*it, i)) {
            Sbeta.erase(i);
            useful = true;
          }
        }
        if (useful) {
          Kbeta.insert(*it);
        }
      }
    }
#ifdef VERBOSE
   cout << ">>>Kbeta = {";
   bool first = true;
   for (set<int>::const_iterator it = Kbeta.begin(); it != Kbeta.end(); it++) {
      vector<int> seq = (*fsm.stateCover.find(*it)).second;
      if (!first) {
          cout << ", ";
      } else {
          first = false;
      }
      cout << "[";
      for (vector<int>::const_iterator itS = seq.begin(); itS != seq.end(); itS++) {
        cout << " " << fsm.intToInput[*itS];
      }
      cout << "]";
   }
   cout << "}" << endl;
    for (set<int>::const_iterator itRB1 = Rbeta->begin(); itRB1 != Rbeta->end(); itRB1++) {
      int p1 = *itRB1;
      for (vector<GraphNode *>::const_iterator it1 = chains[p1].begin(); it1 != chains[p1].end(); it1++) {
        GraphNode * state = *it1;
        printSeqFromLastWithStates(">>>sequences in the chains: ", state);
        cout << endl;
      }
    }
#endif
#ifdef VERBOSE
   set<GraphNode *> set1;
#endif
      for (set<int>::const_iterator it1 = Kbeta.begin(); it1 != Kbeta.end(); it1++) {
        vector<int> seq = (*fsm.stateCover.find(*it1)).second;
        GraphNode * state = addNode(parent, seq);

        for (set<int>::const_iterator it2 = Kbeta.begin(); it2 != Kbeta.end(); it2++) {
          if (*it1 != *it2) {
            const vector<int> &seq = fsm.getGamma(*it1, *it2);
#ifdef VERBOSE
  GraphNode *rState = 
#endif
              addNode(state, seq);
#ifdef VERBOSE
  set1.insert(rState);
#endif
          }
        }
      }
#ifdef VERBOSE
     cout << ">>>set1 = {";
     for (set<GraphNode *>::const_iterator it = set1.begin(); it != set1.end(); it++) {
       cout << "[";
       printSeqFromLast("", *it);
       cout << "]";
     }
     cout << "}" << endl;
#endif
        
#ifdef VERBOSE
        set<GraphNode *> set2;
#endif
      for (set<int>::const_iterator it1 = Kbeta.begin(); it1 != Kbeta.end(); it1++) {
        vector<int> seq = (*fsm.stateCover.find(*it1)).second;
        GraphNode * state = addNode(parent, seq);
        // Set 2
    for (set<int>::const_iterator itRB = Rbeta->begin(); itRB != Rbeta->end(); itRB++) {
      set<int> Sbeta = *Rbeta;
      int p = *itRB;
        for (vector<GraphNode *>::const_iterator it2 = chains[p].begin(); it2 != chains[p].end(); it2++) {
          if (*it1 != (*it2) -> state) {
            const vector<int> &seq = fsm.getGamma(*it1, (*it2) -> state);
#ifdef VERBOSE
            GraphNode *rState = 
#endif
              addNode(state, seq);
#ifdef VERBOSE
              set2.insert(rState);
#endif
          }
        }
      }
    }
#ifdef VERBOSE
     cout << ">>>set2 = {";
     for (set<GraphNode *>::const_iterator it = set2.begin(); it != set2.end(); it++) {
       cout << "[";
       printSeqFromLast("", *it);
       cout << "]";
     }
     cout << "}" << endl;
#endif

#ifdef VERBOSE
        set<GraphNode *> set3;
#endif
    for (set<int>::const_iterator itRB = Rbeta->begin(); itRB != Rbeta->end(); itRB++) {
      set<int> Sbeta = *Rbeta;
      int p = *itRB;
      for (vector<GraphNode *>::const_iterator it1 = chains[p].begin(); it1 != chains[p].end(); it1++) {
        GraphNode * state = *it1;

        for (set<int>::const_iterator it2 = Kbeta.begin(); it2 != Kbeta.end(); it2++) {
          if ((*it1) -> state != *it2) {
            const vector<int> &seq = fsm.getGamma((*it1) -> state, *it2);
#ifdef VERBOSE
  GraphNode *rState = 
#endif
              addNode(state, seq);
#ifdef VERBOSE
  set3.insert(rState);
#endif
          }
        }
      }
    }
#ifdef VERBOSE
  cout << ">>>set3 = {";
  for (set<GraphNode *>::const_iterator it = set3.begin(); it != set3.end(); it++) {
    cout << "[";
    printSeqFromLast("", *it);
    cout << "]";
  }
  cout << "}" << endl;
#endif


#ifdef VERBOSE
  set<GraphNode *> set4;
#endif
    for (set<int>::const_iterator itRB1 = Rbeta->begin(); itRB1 != Rbeta->end(); itRB1++) {
      int p1 = *itRB1;
      for (vector<GraphNode *>::const_iterator it1 = chains[p1].begin(); it1 != chains[p1].end(); it1++) {
        GraphNode * state = *it1;
        for (set<int>::const_iterator itRB2 = Rbeta->begin(); itRB2 != Rbeta->end(); itRB2++) {
          int p2 = *itRB2;
          for (vector<GraphNode *>::const_iterator it2 = chains[p2].begin(); it2 != chains[p2].end(); it2++) {
            if ((*it1) -> state != (*it2) -> state) {
              const vector<int> &seq = fsm.getGamma((*it1) -> state, (*it2) -> state);
#ifdef VERBOSE
  GraphNode *rState = 
#endif
              addNode(state, seq);
#ifdef VERBOSE
  set4.insert(rState);
#endif
            }
          }
        }
      }
    }
#ifdef VERBOSE
  cout << ">>>set4 = {";
  for (set<GraphNode *>::const_iterator it = set4.begin(); it != set4.end(); it++) {
    cout << "[";
    printSeqFromLast("", *it);
    cout << "]";
  }
  cout << "}" << endl;
#endif
  }
  delete [] chains;
}

void spanUpToNthLevelAux(const FSM &fsm, Graph *graph, GraphNode * node, int level, int maxLevel) {
  if (level > maxLevel) {
    return;
  } else {
    int state = node -> state;
    for (int i = 0; i < fsm.numInputs; i++) {
      int nextState = fsm.nextState[fsm.calcPos(state,i)];
      if (nextState > -1) {
        graph -> addNode(node, i, nextState);
        spanUpToNthLevelAux(fsm, graph, node -> children[i], level + 1, maxLevel);
      }
    }
  }
}

Graph * spanUpToNthLevel(FSM &fsm, int maxLevel) {
  Graph * graph = new Graph(fsm);
  GraphNode * root = graph -> setRoot(fsm.initialState);
  spanUpToNthLevelAux(fsm, graph, root, 0, maxLevel);
  return graph;
}

long int Graph::totalLength() {
  return totalLengthAux(parent, 0);
}

long int Graph::totalLengthAux(GraphNode *node, int level) {
  long int total = 0;
  if (node) {
    int numChildren = 0;
    for (int i = 0; i < fsm.numInputs; i++) {
      if (node -> children[i]) {
        total += totalLengthAux(node -> children[i], level + 1);
        numChildren++;
      }
    }
    if (numChildren == 0) {
      total += level + 1;
    }
  }
  return total;
}

long int Graph::totalLengthWithoutPrefix() {
  return totalLengthWithoutPrefixAux(parent, 0);
}

long int Graph::totalLengthWithoutPrefixAux(GraphNode *node, int level) {
  long int total = 0;
  if (node) {
    int numChildren = 0;
    for (int i = 0; i < fsm.numInputs; i++) {
      if (node -> children[i]) {
        total += totalLengthWithoutPrefixAux(node -> children[i], level + 1);
        level = 0;
        numChildren++;
      }
    }
    if (numChildren == 0) {
      total += level + 1;
    }
  }
  return total;
}

void Graph::spanTransitionCover() {
  setRoot(fsm.initialState);
  set<int> uncoveredStates = fsm.states;
  queue<GraphNode *> nodes;
  nodes.push(parent);
  while(nodes.size() > 0) {
    GraphNode * n = nodes.front();
    nodes.pop();
    int state = n->state;
    if (uncoveredStates.count(state) > 0) {
      uncoveredStates.erase(state);
      for (int i = 0; i < fsm.numInputs; i++) {
        int nextState = fsm.nextState[fsm.calcPos(state,i)];
        if (nextState > -1) {
          addNode(n, i, nextState);
          nodes.push(n -> children[i]);
        }
      }
    }
  }
}

void Graph::spanReducedStateCounting() {
#ifdef VERBOSE
  cout << ">>>Generating RSC" << endl;
#endif
  setRoot(fsm.initialState);
  set<int> uncoveredStates = fsm.states;
  queue<GraphNode *> nodes;
  nodes.push(parent);
#ifdef VERBOSE
  cout << ">>><hr>" << endl;
  printSeqFromLastWithStates(">>>seq in tc: ", parent);
  cout << endl;
#endif
  for (int i = 0; i < fsm.numStates; i++) {
    const vector<int> &seq = fsm.getGamma(fsm.initialState, i);
    if (seq.size() > 0) {
#ifdef VERBOSE            
      cout << ">>>gamma(" << fsm.intToState[fsm.initialState] << ", " << fsm.intToState[i] << ") = ";
      copy(seq.begin(), seq.end(), ostream_iterator<int>(cout, " "));
      cout << endl;
      GraphNode * ns = 
#endif
      addNode(parent, seq);
#ifdef VERBOSE
      printSeqFromLast(">>> res seq: ", ns);
      cout << endl;
#endif
    }
  }
  while(nodes.size() > 0) {
    GraphNode * n = nodes.front();
    nodes.pop();
    int state = n->state;
    if (uncoveredStates.count(state) > 0) {
      uncoveredStates.erase(state);
      for (int i = 0; i < fsm.numInputs; i++) {
        int nextState = fsm.nextState[fsm.calcPos(state,i)];
        if (nextState > -1) {
          GraphNode * newState = addNode(n, i, nextState);
          nodes.push(newState);
#ifdef VERBOSE
          cout << ">>><hr>" << endl;
          printSeqFromLastWithStates(">>>seq in tc: ", newState);
          cout << endl;
#endif
          for (int i = 0; i < fsm.numStates; i++) {
            const vector<int> &seq = fsm.getGamma(nextState, i);
            if (seq.size() > 0) {
#ifdef VERBOSE            
              cout << ">>>gamma(" << fsm.intToState[nextState] << ", " << fsm.intToState[i] << ") = ";
              copy(seq.begin(), seq.end(), ostream_iterator<int>(cout, " "));
              cout << endl;
              GraphNode * ns = 
#endif
              addNode(newState, seq);
#ifdef VERBOSE
              printSeqFromLast(">>> res seq: ", ns);
              cout << endl;
#endif
            }
          }
        }
      }
    }
  }
//  collectLeafes();
//  for (vector<GraphNode *>::const_iterator it = leafes.begin(); it != leafes.end(); it++) {
//    GraphNode * gn = *it;
//    int state = gn -> state;
//    for (int i = 0; i < fsm.numStates; i++) {
//      const vector<int> &seq = fsm.getGamma(state, i);
//    }
//  }
//  leafes.clear();
//  collectLeafes();
//  for (vector<GraphNode *>::const_iterator it = leafes.begin(); it != leafes.end(); it++) {
//    GraphNode * gn = *it;
//    printSeqFromLastWithStates(">>>seq in tc: ", gn);
//    cout << endl;
//    int state = gn -> state;
//    for (int i = 0; i < fsm.numStates; i++) {
//      const vector<int> &seq = fsm.getGamma(state, i);
//      cout << ">>>gamma(" << fsm.intToState[state] << ", " << fsm.intToState[i] << ") = ";
//      copy(seq.begin(), seq.end(), ostream_iterator<int>(cout, " "));
//      cout << endl;
//      GraphNode * ns = addNode(gn, seq);
//      printSeqFromLast(">>> res seq: ", ns);
//      cout << endl;
//    }
//  }
//  leafes.clear();
}


//salva as sequencias do State Counting
void Graph::saveOn(ostream &os, const string &pre)
{
	collectLeafes(false);
	for (vector<GraphNode *>::const_iterator it = leafes.begin(); it != leafes.end(); it++)
	{
		vector<string> in;
  		getInputSeq(*it, in);

		 for (vector<string>::const_iterator it2 = in.begin(); it2 != in.end(); it2++)
		 {
    		os << *it2;
		 }

		os << endl;
  	
	}
	
}













void Graph::spanReducedStateCountingK(int k) {
#ifdef VERBOSE
  cout << ">>>Generating RSC" << endl;
#endif
  setRoot(fsm.initialState);
  set<int> uncoveredStates = fsm.states;
  queue<GraphNode *> nodes;
  nodes.push(parent);
#ifdef VERBOSE
  cout << ">>><hr>" << endl;
  printSeqFromLastWithStates(">>>seq in tc: ", parent);
  cout << endl;
#endif
  for (int i = 0; i < fsm.numStates; i++) {
    const vector<int> &seq = fsm.getGamma(fsm.initialState, i);
    if (seq.size() > 0) {
#ifdef VERBOSE            
      cout << ">>>gamma(" << fsm.intToState[fsm.initialState] << ", " << fsm.intToState[i] << ") = ";
      copy(seq.begin(), seq.end(), ostream_iterator<int>(cout, " "));
      cout << endl;
      GraphNode * ns = 
#endif
      addNode(parent, seq);
#ifdef VERBOSE
      printSeqFromLast(">>> res seq: ", ns);
      cout << endl;
#endif
    }
  }

  while(nodes.size() > 0) {
    GraphNode * n = nodes.front();
    nodes.pop();
    int state = n->state;
    if (uncoveredStates.count(state) > 0) {
      uncoveredStates.erase(state);
      for (int i = 0; i < fsm.numInputs; i++) {
        int nextState = fsm.nextState[fsm.calcPos(state,i)];
        if (nextState > -1) {   //se existe proximo estado para essa entrada
          GraphNode * newState = addNode(n, i, nextState);
          nodes.push(newState);

#ifdef VERBOSE
          cout << ">>><hr>" << endl;
          printSeqFromLastWithStates(">>>seq in tc: ", newState);
          cout << endl;
#endif
	//Inicio da concatenacao de X 
	  queue<GraphNode *> nodes2;   //adicionada	
	  nodes2.push(newState);   //adicionada
	 for (int j = 0; j < k; j++)		//X vai ser k vezes adicionado a seq do TC
	 { 
	  int tam = nodes2.size();
          cout << "tam: " << tam << endl;  //linha adicionada
	  for(int g = 0; g < tam; g++)	//adicionado
	  {   	
   	   GraphNode * n2 = nodes2.front();		
	   int atual = n2->state;	
	   for (int w = 0; w < fsm.numInputs; w++)
 	   {
            cout << "##Input: " << fsm.intToInput[w] << endl;  //linha adicionada
	    cout << "##ATUAL: " << fsm.intToState[atual] << endl;  //linha adicionada
	    int nextState2 = fsm.nextState[fsm.calcPos(atual,w)];
            cout <<" nextState2 = " << fsm.intToState[nextState2] << endl;   //linha adicionada    		
            if (nextState2 > -1)
	    {			//existe proximo estado para essa entrada?
          	GraphNode * newState2 = addNode(n2, w, nextState2);
          	nodes2.push(newState2);	//empilha

		for (int i = 0; i < fsm.numStates; i++)
		{ //adicionando gamma após TC.X
	            const vector<int> &seq = fsm.getGamma(nextState2, i);
        	    if (seq.size() > 0)
	    	    {
	              GraphNode * ns = addNode(newState2, seq);
		      printSeqFromLast("#### res seq: ", ns);
	              cout << endl;
 		    }
	        }
	    }
	   }
	   nodes2.pop(); 		///adicionada
	   cout << "DELETOU" << endl; 
	  }//fim for 'g' 
	}//fim for 'j'

          for (int i = 0; i < fsm.numStates; i++) {
            const vector<int> &seq = fsm.getGamma(nextState, i);
            if (seq.size() > 0) {    //existe gamma para o par "nextState" e "i"
#ifdef VERBOSE            
              cout << ">>>gamma(" << fsm.intToState[nextState] << ", " << fsm.intToState[i] << ") = ";
              copy(seq.begin(), seq.end(), ostream_iterator<int>(cout, " "));
              cout << endl;
              GraphNode * ns = 
#endif
              addNode(newState, seq);   //adiciona a sequencia que possui gamma
#ifdef VERBOSE
              printSeqFromLast(">>> res seq: ", ns);
              cout << endl;
#endif
            }
          }
        }
      }
    }
  }
/*transition cover

  setRoot(fsm.initialState);
  set<int> uncoveredStates = fsm.states;
  queue<GraphNode *> nodes;
  nodes.push(parent);
  while(nodes.size() > 0) {
    GraphNode * n = nodes.front();
    nodes.pop();
    int state = n->state;
    if (uncoveredStates.count(state) > 0) {
      uncoveredStates.erase(state);
      for (int i = 0; i < fsm.numInputs; i++) {
        int nextState = fsm.nextState[fsm.calcPos(state,i)];
        if (nextState > -1) {
          addNode(n, i, nextState);
          nodes.push(n -> children[i]);
        }
      }
    }
  }
*/
}



GraphNode * Graph::compareOutput(FSM &m1, FSM &m2, long & numEqual, long & numDiff, bool onlyFirst) {
  collectLeafes();
  vector<string> in;
  vector<string> out1;
  vector<string> out2;
  numEqual = 0;
  numDiff  = 0;
  GraphNode * diffNode = NULL;
  for (vector<GraphNode *>::const_iterator it = leafes.begin(); it != leafes.end(); it++) {
    in.clear();
    out1.clear();
    out2.clear();
    getInputSeq(*it, in);
    m1.outputSeq(in, out1);
    m2.outputSeq(in, out2);
    bool diff = (out1 != out2);
#ifdef DEBUG1
      cout << "in = ";
      copy(in.begin(), in.end(), ostream_iterator<string>(cout, " "));
.     cout << endl;
      cout << "out1 = ";
      copy(out1.begin(), out1.end(), ostream_iterator<string>(cout, " "));
      cout << endl;
      cout << "out2 = ";
      copy(out2.begin(), out2.end(), ostream_iterator<string>(cout, " "));
      cout << endl;
      cout << "diff = " << diff << endl;
#endif
    if (diff) {
      if (diffNode == NULL) {
        diffNode = *it;
      }
      numDiff++;
      if (onlyFirst) {
        break;
      }
    } else {
      numEqual++;
    }
  }
  return diffNode;
}
