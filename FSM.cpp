
#include <iostream>
#include <set>
#include <map>
#include <queue>
#include <vector>
#include <stdlib.h>
#include <algorithm>
#include "FSM.h"

FSM::FSM() {
  numStates = 0;
  numInputs = 0;
  numOutputs = 0;
  initialState = -1;
  nextState = NULL;
  output = NULL;
}

FSM::FSM(const FSM &m) : 
  initialState(m.initialState),
  stateToInt(m.stateToInt),
  intToState(m.intToState),
  numStates(m.numStates),
  inputToInt(m.inputToInt),
  intToInput(m.intToInput),
  numInputs(m.numInputs),
  outputToInt(m.outputToInt),
  intToOutput(m.intToOutput),
  numOutputs(m.numOutputs),
  quasiEq(m.quasiEq),
  qeRel(m.qeRel),
  coreStates(m.coreStates),
  coreCover(m.coreCover),
  stateCover(m.stateCover),
  gamma(m.gamma),
  R(m.R),
  states(m.states) {
    nextState = new int[numStates*numInputs];
    output = new int[numStates*numInputs];
    for(int i = 0; i < numStates*numInputs; i++) {
      nextState[i] = m.nextState[i];
      output[i] = m.output[i];
    }
}

FSM::~FSM() {
    if (nextState != NULL) {
      delete nextState;
    }
    if (output != NULL) {
      delete output;
    }
}

bool FSM::isReduced() {
  for (int i = 0; i < numStates; i++) {
    for (int j = i+1; j < numStates; j++) {
      vector<int> & vec = gamma[make_pair(i,j)];
      if (vec.size() == 0) {
        return false;
      }
    }
  }
  return true;
}

void FSM::addTempTransition(const string &si, const string &in, const string &out, const string &sj){
  TempTrans tt;
  if (stateToInt.count(si) > 0) {
    tt.si = stateToInt[si];
  } else {
    tt.si = stateToInt[si] = numStates++;
    intToState[tt.si] = si;
  }
  if (initialState == -1) {
    initialState = tt.si;
  }
  if (inputToInt.count(in) > 0) {
    tt.in = inputToInt[in];
  } else {
    tt.in = inputToInt[in] = numInputs++;
    intToInput[tt.in] = in;
  }
  if (outputToInt.count(out) > 0) {
    tt.out = outputToInt[out];
  } else {
    tt.out = outputToInt[out] = numOutputs++;
    intToOutput[tt.out] = out;
  }
  if (stateToInt.count(sj) > 0) {
    tt.sj = stateToInt[sj];
  } else {
    tt.sj = stateToInt[sj] = numStates++;
    intToState[tt.sj] = sj;
  }
  tempTransitions.insert(tempTransitions.end(), tt);
}

void FSM::initFSM() {
  for(int i = 0; i < numStates*numInputs; i++) {
    output[i] = 0;
    nextState[i] = 0;
  }
  output[numStates*numInputs-1]--;
}

bool FSM::incFSM() {
  int posOut = numStates*numInputs - 1;
  while (posOut >= 0 && output[posOut] == numOutputs - 1) {
    output[posOut] = 0;
    posOut--;
  }
  if (posOut >= 0) {
    output[posOut]++;
  } else {
    posOut = 0;
    int posNS = numStates*numInputs - 1;
    while (posNS >= 0 && nextState[posNS] == numStates - 1) {
      nextState[posNS]=0;
      posNS--;
    }
    if (posNS >= 0) {
      nextState[posNS]++;
    } else {
      return false;
    }
  }
  return true;
}

void FSM::generateRandom(int ns, map<int, string> &smap, int ni, map<int, string> &imap, int no, map<int, string> &omap) {
  numStates = ns;
  for (int i = 0; i < ns; i++) {
    string buf = smap[i];
    intToState[i] = buf;
    stateToInt[buf] = i;
  }
  numInputs = ni;
  for (int i = 0; i < ni; i++) {
    string buf = imap[i];
    intToInput[i] = buf;
    inputToInt[buf] = i;
  }
  numOutputs = no;
  for (int i = 0; i < no; i++) {
    string buf = omap[i];
    intToOutput[i] = buf;
    outputToInt[buf] = i;
  }
    if (nextState != NULL) {
      delete nextState;
    }
    if (output != NULL) {
      delete output;
    }
  nextState = new int[numStates*numInputs];
  output = new int[numStates*numInputs];

  for(int i = 0; i < numStates*numInputs; i++) {
    output[i] = -1;
    nextState[i] = -1;
  }

  set<int> R;
  initialState = 0;
  map<int, set<int> > undefs;
  for (int i = 0; i < ns; i++) {
    states.insert(i);
    for (int j = 0; j < ni; j++) {
      undefs[i].insert(j);
    }
  }
  set<int> U = states;
  U.erase(initialState);
  R.insert(initialState);
  while(U.size() > 0) {
    int Rsize = R.size();
    int Rran = rand() % Rsize;
    set<int>::const_iterator Rel = R.begin();
    for (int i = 0; i < Rran; i++, Rel++) {
      // Nothing
    }
    int s = *Rel;

    int inpsize = undefs[s].size();
    int inpran  = rand() % inpsize;
    set<int>::const_iterator inpel = undefs[s].begin();
    for (int i = 0; i < inpran; i++, inpel++) {
      // Nothing
    }
    int inp = *inpel;
    undefs[s].erase(inp);
    if (undefs[s].size() == 0) {
      R.erase(s);
    }

    int Usize = U.size();
    int Uran  = rand() % Usize;
    set<int>::const_iterator Uel = U.begin();
    for (int i = 0; i < Uran; i++, Uel++) {
      // Nothing
    }
    int sl = *Uel;
    U.erase(sl);
    R.insert(sl);

    int out = rand() % no;

    int pos = calcPos(s, inp);
    nextState[pos] = sl;
    output[pos] = out;
  }
}

void FSM::generateRandom(int ns, int ni, int no) {
  char buf[1024];
  numStates = ns;
  for (int i = 0; i < ns; i++) {
    sprintf(buf, "%d", i);
    intToState[i] = buf;
    stateToInt[buf] = i;
  }
  numInputs = ni;
  for (int i = 0; i < ni; i++) {
    sprintf(buf, "%d", i);
    intToInput[i] = buf;
    inputToInt[buf] = i;
  }
  numOutputs = no;
  for (int i = 0; i < no; i++) {
    sprintf(buf, "%d", i);
    intToOutput[i] = buf;
    outputToInt[buf] = i;
  }
    if (nextState != NULL) {
      delete nextState;
    }
    if (output != NULL) {
      delete output;
    }
  nextState = new int[numStates*numInputs];
  output = new int[numStates*numInputs];

  for(int i = 0; i < numStates*numInputs; i++) {
    output[i] = -1;
    nextState[i] = -1;
  }

  set<int> R;
  initialState = 0;
  map<int, set<int> > undefs;
  for (int i = 0; i < ns; i++) {
    states.insert(i);
    for (int j = 0; j < ni; j++) {
      undefs[i].insert(j);
    }
  }
  set<int> U = states;
  U.erase(initialState);
  R.insert(initialState);
  while(U.size() > 0) {
    int Rsize = R.size();
    int Rran = rand() % Rsize;
    set<int>::const_iterator Rel = R.begin();
    for (int i = 0; i < Rran; i++, Rel++) {
      // Nothing
    }
    int s = *Rel;

    int inpsize = undefs[s].size();
    int inpran  = rand() % inpsize;
    set<int>::const_iterator inpel = undefs[s].begin();
    for (int i = 0; i < inpran; i++, inpel++) {
      // Nothing
    }
    int inp = *inpel;
    undefs[s].erase(inp);
    if (undefs[s].size() == 0) {
      R.erase(s);
    }

    int Usize = U.size();
    int Uran  = rand() % Usize;
    set<int>::const_iterator Uel = U.begin();
    for (int i = 0; i < Uran; i++, Uel++) {
      // Nothing
    }
    int sl = *Uel;
    U.erase(sl);
    R.insert(sl);

    int out = rand() % no;

    int pos = calcPos(s, inp);
    nextState[pos] = sl;
    output[pos] = out;
  }
}

void FSM::addRandomTransition() {
  int numUndef = 0;
  map<int, int> m;
  for(int i = 0; i < numStates*numInputs; i++) {
    if (nextState[i] == -1) {
      m[numUndef++] = i;
    }
  }
  if (numUndef > 0) {
    int ran = rand() % numUndef;
    int pos = m[ran];
    int sl  = rand() % numStates;
    int out = rand() % numOutputs;
    nextState[pos] = sl;
    output[pos] = out;
  }
}

void FSM::generateRandomFull(int ns, int ni, int no) {
  generateRandom(ns, ni, no);
  randomFulfill();
}

void FSM::generateRandomFull(int ns, map<int, string> &smap, int ni, map<int, string> &imap, int no, map<int, string> &omap) {
  generateRandom(ns, smap, ni, imap, no, omap);
  randomFulfill();
}

void FSM::randomFulfill() {
  int numDef = 0;
  for(int i = 0; i < numStates*numInputs; i++) {
    if (nextState[i] > -1) {
      numDef++;
    }
  }
  for (int i = numDef - 1; i < numStates * numInputs; i++) {
    addRandomTransition();
  }
}

void FSM::printOn(ostream &os, const string &pre) {
  for (int i = 0; i < numStates; i++) {
    for (int j = 0; j < numInputs; j++) {
      int pos = calcPos(i, j);
      if (nextState[pos] > -1) {
        os << pre << intToState[i] << " -- " << intToInput[j] << " / " << intToOutput[output[pos]] << " -> " << intToState[nextState[pos]] << endl;
      }
    }
  }
}

void FSM::imprime(vector<int> pre) {			//metodo adicionado (Lucio)
///  vector<int>::iterator i; 
//  for( i = pre.begin(); i != pre.end() ; i++ )
 //    cout << "seq: " *i << endl;

for(vector<int>::size_type i = 0; i < pre.size(); i++)
    cout << "seq: " << pre[i] << endl;
}

void FSM::printDotOn(ostream &os, const string &pre) {
  os << pre << "digraph FSM {" << endl;
  for (int i = 0; i < numStates; i++) {
    for (int j = 0; j < numInputs; j++) {
      int pos = calcPos(i, j);
      if (nextState[pos] > -1) {
        os << pre << intToState[i] << " -> " << intToState[nextState[pos]] << " [label=\"" << intToInput[j] << " / " << intToOutput[output[pos]] << "\"];" << endl;
      }
    }
  }
  os << pre << "}" << endl;
}

int FSM::shortestSequenceDistance(FSM &m) {
  set<pair<int, int> > visited;
  queue<pair<int, pair<int, int> > > toVisit;
  bool qe = true;
  toVisit.push(make_pair(1, make_pair(initialState, m.initialState)));
  while(toVisit.size() > 0 && qe) {
    const pair<int, pair<int, int> > &p1 = toVisit.front();
    int d = p1.first;
    int s1 = p1.second.first;
    int s2 = p1.second.second;
    toVisit.pop();
    for (int i1 = 0; i1 < numInputs; i1++) {
      const string & in1 = intToInput[i1];
      int pos1 = calcPos(s1, i1);
      int o1 = output[pos1];
      if (o1 > -1) {
        const string & out1 = intToOutput[o1];
        if (m.inputToInt.count(in1) > 0) {
          int i2 = m.inputToInt[in1];
          int pos2 = m.calcPos(s2, i2);
          int o2 = m.output[pos2];
          if (o2 > -1) {
            const string & out2 = m.intToOutput[o2];
            if (out1 == out2) {
              int ns1 = nextState[pos1];
              int ns2 = m.nextState[pos2];
              const pair<int, int> & p2 = make_pair(ns1, ns2);
              if (visited.count(p2) == 0) {
                visited.insert(p2);
                toVisit.push(make_pair(d + 1, p2));
              }
            } else {
              return d;
            }
          } else {
            return d;
          }
        }
      }
    }
  }
  return 0;
}
//metodo que verifica se as fsms sao quasi-equivalentes ou iguais?
bool FSM::isQuasiEq(FSM &m) {
  set<pair<int, int> > visited;
  queue<pair<int, int> > toVisit;
  bool qe = true;
  toVisit.push(make_pair(initialState, m.initialState));
  while(toVisit.size() > 0 && qe) {
    const pair<int, int> &p1 = toVisit.front();
    int s1 = p1.first;
    int s2 = p1.second;
    toVisit.pop();
    for (int i1 = 0; i1 < numInputs; i1++) {
      const string & in1 = intToInput[i1];
      int pos1 = calcPos(s1, i1);
      int o1 = output[pos1];
      if (o1 > -1) {
        const string & out1 = intToOutput[o1];
        if (m.inputToInt.count(in1) > 0) {
          int i2 = m.inputToInt[in1];
          int pos2 = m.calcPos(s2, i2);
          int o2 = m.output[pos2];
          if (o2 > -1) {
            const string & out2 = m.intToOutput[o2];
            if (out1 == out2) {
              int ns1 = nextState[pos1];
              int ns2 = m.nextState[pos2];
              const pair<int, int> & p2 = make_pair(ns1, ns2);
              if (visited.count(p2) == 0) {
                visited.insert(p2);
                toVisit.push(p2);
              }
            } else {
              //cout << " 209" << endl;
              //cout << "(" << intToState[s1] << ", " << intToInput[i1] << ") = " << out1 << endl;
              //cout << "(" << m.intToState[s2] << ", " << m.intToInput[i2] << ") = " << out2 << endl;
              qe = false;
              break;
            }
          } else {
              //cout << " 214" << endl;
            qe = false;
            break;
          }
        } else {
              //cout << " 219" << endl;
          qe = false;
          break;
        }
      }
    }
  }
  return qe;
}

int FSM::outputDistance(FSM &m) {
  set<pair<int, int> > visited;
  queue<pair<int, int> > toVisit;
  int dist = 0;
  toVisit.push(make_pair(initialState, m.initialState));
  while(toVisit.size() > 0) {
    const pair<int, int> &p1 = toVisit.front();
    int s1 = p1.first;
    int s2 = p1.second;
    toVisit.pop();
    for (int i1 = 0; i1 < numInputs; i1++) {
      const string & in1 = intToInput[i1];
      int pos1 = calcPos(s1, i1);
      int o1 = output[pos1];
      if (o1 > -1) {
        const string & out1 = intToOutput[o1];
        if (m.inputToInt.count(in1) > 0) {
          int i2 = m.inputToInt[in1];
          int pos2 = m.calcPos(s2, i2);
          int o2 = m.output[pos2];
          if (o2 > -1) {
            const string & out2 = m.intToOutput[o2];
            if (out1 != out2) {
              dist++;
            }
          } else {
            dist++;
          }
          int ns1 = nextState[pos1];
          int ns2 = m.nextState[pos2];
          const pair<int, int> & p2 = make_pair(ns1, ns2);
          if (visited.count(p2) == 0) {
            visited.insert(p2);
            toVisit.push(p2);
          }
        }
      }
    }
  }
  return dist;
}

void FSM::endUpTransitions() {
    if (nextState != NULL) {
      delete nextState;
    }
    if (output != NULL) {
      delete output;
    }
  nextState = new int[numStates*numInputs];
  output = new int[numStates*numInputs];

  for(int i = 0; i < numStates*numInputs; i++) {
    output[i] = -1;
    nextState[i] = -1;
  }

  for(vector<TempTrans>::const_iterator it = tempTransitions.begin(); it != tempTransitions.end(); it++) {
    const TempTrans & tt = *it;

    int pos = calcPos(tt.si, tt.in);
    output[pos] = tt.out;
    nextState[pos] = tt.sj;
  }

  for (int i = 0; i < numStates; i++) {
    states.insert(i);
  }
}

void FSM::setup() {
//  quasiEq.clear();
//  qeRel.clear();
//  coreStates.clear();
//  coreCover.clear();
//  stateCover.clear();
//  gamma.clear();
//  R.clear();
  bool * qe = new bool[numStates * numStates];
  qeRel.clear();
//  cout << "calculating quasi-relation..." << endl;
  for (int i = 0; i < numStates * numStates; i++) {
    qe[i] = true;
  }
  for (int i = 0; i < numStates; i++) {
    for (int j = 0; j < numStates; j++) {
      int posIJ = i*numStates+j;
      bool equal = true;
      for (int k = 0; k < numInputs && equal; k++) {
        int posJK = calcPos(j, k);
        if (output[posJK] >= 0) {
          int posIK = calcPos(i, k);
          equal = (output[posJK] == output[posIK]);
        }
      }
      qe[posIJ] = equal;
    }
  }
  int numChanges;
  do {
    numChanges = 0;
    for (int i = 0; i < numStates; i++) {
      for (int j = 0; j < numStates; j++) {
        int posIJ = i*numStates+j;
        if (qe[posIJ]) {
          bool equal = true;
          for (int k = 0; k < numInputs && equal; k++) {
            int nextJK = nextState[calcPos(j, k)];
            if (nextJK >= 0) {
              int nextIK = nextState[calcPos(i, k)];
              equal = qe[nextIK*numStates+nextJK];
            }
          }
          if (! equal) {
            numChanges ++;
            qe[posIJ] = equal;
          }
        }
      }
    }
  } while (numChanges > 0);

  for (int i = 0; i < numStates; i++) {
    for (int j = 0; j < numStates; j++) {
      qeRel[make_pair(i,j)] = (qe[i*numStates + j]);
    }
  }

  // Calculating the state core
  coreStates.clear();
//  cout << "calculating state core..." << endl;
  set<int> notIncluded;
  int * numCover = new int[numStates];
  for (int i = 0; i < numStates; i++) {
    notIncluded.insert(i);
    numCover[i] = 0;
  }
  for (int i = 0; i < numStates; i++) {
    for (int j = 0; j < numStates; j++) {
      if (isQE(i,j)) {
        numCover[i]++;
      }
    }
  }
  bool alreadyIncludedInitialState = false;
  while(notIncluded.size() > 0) {
    int candidate;
    if (alreadyIncludedInitialState) {
      set<int>::const_iterator it = notIncluded.begin();
      candidate = *it;
      it ++;
      for (; it != notIncluded.end(); it++) {
        int thisState = *it;
        if (numCover[candidate] < numCover[thisState]) {
          candidate = thisState;
        }
      }
    } else {
      candidate = initialState;
      alreadyIncludedInitialState = true;
    }
    notIncluded.erase(candidate);
    coreStates.insert(candidate);
    bool changed;
    do {
      changed = false;
      for (set<int>::iterator itNI = notIncluded.begin(); !changed && itNI != notIncluded.end(); itNI++) {
        for (set<int>::iterator itCS = coreStates.begin(); !changed && itCS != coreStates.end(); itCS++) {
          int removeState = *itNI;
          if (isQE(*itCS, removeState)) {
            notIncluded.erase(removeState);
            changed = true;
	    break;
          }
        }
	if (changed) {
	  break;
	}
      }
    } while (changed);
  }
  
  // Calculating the core cover
  coreCover.clear();
//  cout << "calculating core cover..." << endl;
  set<int> toBeCovered = coreStates;
  set<int> VorS;
  queue<pair<int, vector<int> > > qu;
  vector<int> emptySeq;
  qu.push(make_pair(initialState, emptySeq));
  VorS.insert(initialState);
  while(qu.size() > 0 && toBeCovered.size() > 0) {
    const pair<int, vector<int> > pr = qu.front();
    qu.pop();
    const int state = pr.first;
    const vector<int> seq = pr.second;
    if (toBeCovered.count(state) > 0) {
      toBeCovered.erase(state);
      coreCover.insert(seq);
      stateCover[state] = seq;
    }
    for (int k = 0; k < numInputs; k++) {
      int pos = calcPos(state, k);
      int next = nextState[pos];
      if (next > -1 and VorS.count(next) == 0) {
        VorS.insert(next);
        vector<int> newSeq = seq;
        newSeq.push_back(k);
        qu.push(make_pair(next, newSeq));
      }
    }
  }

  gamma.clear();
//  cout << "calculating gamma..." << endl;
  vector<int> emptyVector;
  for (int i = 0; i < numStates; i++) {
    for (int j = i+1; j < numStates; j++) {
      gamma[make_pair(i,j)] = emptyVector;
      vector<int> & vec = gamma[make_pair(i,j)];
      bool equal = true;
      for (int k = 0; k < numInputs && equal; k++) {
        int outputIK = output[calcPos(i, k)];
        int outputJK = output[calcPos(j, k)];
        if (outputIK >= 0 && outputJK >= 0 && outputIK != outputJK) {
          equal = false;
          vec.insert(vec.begin(), k);
        }
      }
    }
  }
  do {
    numChanges = 0;
    for (int i = 0; i < numStates; i++) {
      for (int j = i+1; j < numStates; j++) {
        vector<int> & vec = gamma[make_pair(i,j)];
        if (vec.size() == 0) {
          bool equal = true;
          for (int k = 0; k < numInputs && equal; k++) {
            int nextIK = nextState[calcPos(i, k)];
            int nextJK = nextState[calcPos(j, k)];
            if (nextIK >= 0 && nextJK >= 0) {
              if (nextIK > nextJK) {
                int swp = nextIK;
                nextIK = nextJK;
                nextJK = swp;
              }
              vector<int> &vecIJ = gamma[make_pair(nextIK, nextJK)];
              if (vecIJ.size() > 0) {
                vec.insert(vec.begin(), k);
                equal = false;
                numChanges ++;
                for (vector<int>::const_iterator it = vecIJ.begin(); it != vecIJ.end(); it++) {
                  vec.insert(vec.end(), *it);
                }
              }
            }
          }
        }
      }
    }
  } while(numChanges > 0);

  // Calculating R (set of sets of pairwisely distinguishable states)
//  cout << "calculating R..." << endl;
//  for (int j = 0; j < numStates; j++) {
//    set<int> Rcurr;
//    Rcurr.insert(j);
//    R.insert(Rcurr);
//  }
//  int oldSize;
//  int newSize = R.size();
//  set<set<int> > toRemove;
//  set<pair<set<int>, set<int> > > usedPairs;
//  do {
//    numChanges = 0;
//    set<set<int> > toInclude;
//    for (set<set<int> >::const_iterator itR1 = R.begin(); itR1 != R.end(); itR1++) {
//      set<int> Rc1 = *itR1;
//      for (set<set<int> >::const_iterator itR2 = R.begin(); itR2 != R.end(); itR2++) {
//        set<int> Rc2 = *itR2;
//        if (Rc1 < Rc2 && usedPairs.count(make_pair(Rc1, Rc2)) == 0) {
//          usedPairs.insert(make_pair(Rc1, Rc2));
//          bool pairwiseDistinguish = true;
//          for (set<int>::const_iterator itRc1 = Rc1.begin(); itRc1 != Rc1.end() && pairwiseDistinguish; itRc1++) {
//            for (set<int>::const_iterator itRc2 = Rc2.begin(); itRc2 != Rc2.end() && pairwiseDistinguish; itRc2++) {
//              int si, sj;
//              if (*itRc1 > *itRc2) {
//                si = *itRc2;
//                sj = *itRc1;
//              } else {
//                si = *itRc1;
//                sj = *itRc2;
//              }
//              if (gamma[make_pair(si, sj)].size() == 0) {
//                pairwiseDistinguish = false;
//              }
//            }
//          }
//          if (pairwiseDistinguish) {
//            set<int> Rc12;
//            set_union(Rc1.begin(), Rc1.end(), Rc2.begin(), Rc2.end(), insert_iterator<set<int> >(Rc12, Rc12.end()));
//            toRemove.insert(Rc1);
//            toRemove.insert(Rc2);
//            toInclude.insert(Rc12);
//            numChanges++;
//          }
//        }
//      }
//    }
//    set<set<int> > temp;
//    copy(R.begin(), R.end(), insert_iterator<set<set<int> > >(temp, temp.end()));
//    R.clear();
//    set_union(temp.begin(), temp.end(), toInclude.begin(), toInclude.end(), insert_iterator<set<set<int> > >(R, R.end()));
//    //R.insert(Rcurr);
//    oldSize = newSize;
//    newSize = R.size();
//  } while(newSize > oldSize);
//  set<set<int> > temp;
//  set_difference(R.begin(), R.end(), toRemove.begin(), toRemove.end(), insert_iterator<set<set<int> > >(temp, temp.end()));
//  R.clear();
//  copy(temp.begin(), temp.end(), insert_iterator<set<set<int> > >(R, R.end()));

  R.clear();
//  cout << "calculating R..." << endl;
  //set<set<int> > R2;
  for (int i = 0; i < numStates; i++) {
    set<int> ss;
    set<int> possible;
    map<int, set<int> > problems;
    ss.insert(i);
    for (int j = 0; j < numStates; j++) {
      if (i != j) {
        if (getGamma(i,j).size() > 0) {
          possible.insert(j);
        }
      }
    }
    while (possible.size() > 0) {
      for (set<int>::const_iterator itP1 = possible.begin(); itP1 != possible.end(); itP1++) {
        int s1 = *itP1;
        problems[s1].clear();
        for (set<int>::const_iterator itP2 = possible.begin(); itP2 != possible.end(); itP2++) {
          int s2 = *itP2;
          if (getGamma(s1, s2).size() == 0) {
            problems[s1].insert(s2);
          }
        }
      }
      set<int>::const_iterator itP = possible.begin();
      int s = *itP++;
      for (; itP != possible.end(); itP++) {
        if (problems[s].size() > problems[*itP].size()) {
          s = *itP;
        }
      }
      ss.insert(s);
      possible.erase(s);
      const set<int> &problemsS = problems[s];
      for (set<int>::const_iterator itPr = problemsS.begin(); itPr != problemsS.end(); itPr++) {
        possible.erase(*itPr);
      }
    }
    R.insert(ss);
  }

//  for (int i = 0; i < numStates; i++) {
//    cout << i << " = " << intToState[i] << endl;
//  }
//  for (int i = 0; i < numStates; i++) {
//    for (int j = i+1; j < numStates; j++) {
//      cout << "(" << intToState[i] << ", " << intToState[j] << ")";
//      vector<int> & vec = gamma[make_pair(i,j)];
//      for(vector<int>::const_iterator it = vec.begin(); it != vec.end(); it++) {
//        cout << " " << intToInput[*it];
//      }
//      cout << endl;
//    }
//  }
//
////  cout << "R";
//  for (set<set<int> >::const_iterator itR = R.begin(); itR != R.end(); itR++) {
//    cout << "{";
//    const set<int> & Rcurr = *itR;
//    for (set<int>::const_iterator itRC = Rcurr.begin(); itRC != Rcurr.end(); itRC++) {
//      cout << " " << intToState[*itRC];
//    }
//    cout << "}";
//  }
//  cout << endl;
////  cout << "R2";
////  for (set<set<int> >::const_iterator itR = R2.begin(); itR != R2.end(); itR++) {
////    cout << "{";
////    const set<int> & Rcurr = *itR;
////    for (set<int>::const_iterator itRC = Rcurr.begin(); itRC != Rcurr.end(); itRC++) {
////      cout << " " << intToState[*itRC];
////    }
////    cout << "}";
////  }
////  cout << endl;
//  for (int i = 0; i < numStates; i++) {
//    for (int j = 0; j < numStates; j++) {
//      if (isQE(i, j)) {
//        cout << intToState[i] << " ]= " << intToState[j] << endl;
//      }
//    }
//  }
//  for (set<int>::const_iterator it = coreStates.begin(); it != coreStates.end(); it++) {
//    cout << " " << intToState[*it];
//  }
//  cout << endl;
//
//  for (set<vector<int> >::const_iterator it = coreCover.begin(); it != coreCover.end(); it++) {
//    const vector<int> & seq = *it;
//    cout << "CoreSeq:";
//    for (vector<int>::const_iterator itS = seq.begin(); itS != seq.end(); itS++) {
//      cout << " " << intToInput[*itS];
//    }
//    cout << endl;
//  }

  delete []numCover;
  delete []qe;
}

bool FSM::outputSeq(const vector<int> &in, vector<string> &out) {
  int s = initialState;
  for (vector<int>::const_iterator it = in.begin(); it != in.end(); it++) {
    int i = *it;
    int pos = calcPos(s, i);
    int sl = nextState[pos];
    if (sl > -1) {
      int o = output[pos];
      if (o > -1) {
        out.insert(out.end(), intToOutput[o]);
        s = sl;
      } else {
        return false;
      }
    } else {
      return false;
    }
  }
  return true;
}

void FSM::debug(ostream &os) {
  os << endl;
  for (int i = 0; i < numStates; i++) {
    for (int j = 0; j < numStates; j++) {
      if (i != j && isQE(i, j)) {
        os << ">>>" << intToState[i] << " ]= " << intToState[j] << endl;
      }
    }
  }
//  os << "R";
  os << ">>>" << "R = ";
  for (set<set<int> >::const_iterator itR = R.begin(); itR != R.end(); itR++) {
    os << "{";
    const set<int> & Rcurr = *itR;
    for (set<int>::const_iterator itRC = Rcurr.begin(); itRC != Rcurr.end(); itRC++) {
      os << " " << intToState[*itRC];
    }
    os << "}";
  }
  os << endl;
  os << ">>>" << "K = {";
  for (set<int>::const_iterator it = coreStates.begin(); it != coreStates.end(); it++) {
    os << " " << intToState[*it];
  }
  os << "}" << endl;

  for (set<vector<int> >::const_iterator it = coreCover.begin(); it != coreCover.end(); it++) {
    const vector<int> & seq = *it;
    os << ">>>" << "CoreSeq:";
    for (vector<int>::const_iterator itS = seq.begin(); itS != seq.end(); itS++) {
      os << " " << intToInput[*itS];
    }
    os << endl;
  }
  for (int i = 0; i < numStates; i++) {
    for (int j = i+1; j < numStates; j++) {
      os << ">>>gamma(" << intToState[i] << ", " << intToState[j] << ")";
      vector<int> & vec = gamma[make_pair(i,j)];
      for(vector<int>::const_iterator it = vec.begin(); it != vec.end(); it++) {
        os << " " << intToInput[*it];
      }
      os << endl;
    }
  }
}

bool FSM::outputSeq(const vector<string> &in, vector<string> &out) {
  vector<int> iin;
  for (vector<string>::const_iterator it = in.begin(); it != in.end(); it++) {
    const string & i = *it;
    if (inputToInt.count(i) > 0) {
      iin.insert(iin.end(), inputToInt[i]);
    } else {
      return false;
    }
  }
  return outputSeq(iin, out);
}

FSM::operator BigNumber() const {
  BigNumber bs = 0;
  long so = (numStates + 1) * (numOutputs + 1);
  for (int i = 0; i < numStates*numInputs; i++) {
    bs = bs * so;
    bs += (nextState[i] + 1)*(numOutputs + 1) + (output[i] + 1);
  }
  return bs;
}

void FSM::readFromBigNumber(int ns, int ni, int no, const BigNumber &bn) {
  char buf[1024];
  numStates = ns;
  for (int i = 0; i < ns; i++) {
    sprintf(buf, "%d", i);
    intToState[i] = buf;
    stateToInt[buf] = i;
    states.insert(i);
  }
  numInputs = ni;
  for (int i = 0; i < ni; i++) {
    sprintf(buf, "%d", i);
    intToInput[i] = buf;
    inputToInt[buf] = i;
  }
  numOutputs = no;
  for (int i = 0; i < no; i++) {
    sprintf(buf, "%d", i);
    intToOutput[i] = buf;
    outputToInt[buf] = i;
  }
  if (nextState != NULL) {
    delete nextState;
  }
  nextState = new int[numStates*numInputs];
  if (output != NULL) {
    delete output;
  }
  output = new int[numStates*numInputs];


  for(int i = 0; i < numStates*numInputs; i++) {
    output[i] = -1;
    nextState[i] = -1;
  }

  BigNumber so = (numStates + 1) * (numOutputs + 1);
  BigNumber bs = bn;
  long no1 = numOutputs + 1;
  for (int i = numStates*numInputs - 1; i >= 0; i--) {
    long val = (long) (bs % so);
    bs = bs / so;
    output[i] = val % no1 - 1;
    nextState[i] = val / no1 - 1;
  }
  initialState = 0;
  setup();
}
//metodo que verifica se a fsm possui 2 transicoes repetidas
bool FSM::sameInputOutputPairs() {
  for (int i = 0; i < numInputs; i++) {
    int out = -1;
    for (int s = 0; s < numStates; s++) {
      int pos = calcPos(s, i);
      if (output[pos] >= 0) {
        if (out >= 0) {
          if (output[pos] != out) {
            return false;
          }
        } else {
          out = output[pos];
        }
      }
    }
  }
  return true;
}
