#ifndef __FSM_H__
#define __FSM_H__

#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <bits/stl_pair.h>

#include "BigNumber.h"

using namespace std;

enum {OUTPUTDIST = 0, LENMINDIST = 1};

class FSM {
  public:
    FSM();
    FSM(const FSM &);
    ~FSM();

    bool isReduced(); //deve ser chamada depois do metodo setup()
    void imprime(vector<int> pre);  //método adicionado (Lucio)
    void printOn(ostream &, const string &prefix = "");
    void printDotOn(ostream &, const string &prefix = "");
    void generateRandom(int ns, int ni, int no);
    void generateRandom(int ns, map<int, string> &smap, int ni, map<int, string> &imap, int no, map<int, string> &omap);
    void generateRandomFull(int ns, int ni, int no);
    void generateRandomFull(int ns, map<int, string> &smap, int ni, map<int, string> &imap, int no, map<int, string> &omap);
    void randomFulfill();
    void addRandomTransition();
    void addTempTransition(const string &, const string &, const string &, const string &);
    void initFSM();
    bool incFSM();
    bool sameInputOutputPairs();
    void endUpTransitions();
    bool isQuasiEq(FSM &);
    int outputDistance(FSM &);
    int shortestSequenceDistance(FSM &);
    bool outputSeq(const vector<int> &, vector<string> &);
    bool outputSeq(const vector<string> &, vector<string> &);
    void setup();
    void debug(ostream &);
    operator BigNumber () const;
    void readFromBigNumber(int ns, int ni, int no, const BigNumber &bn);
    int getNumInputs() {
      return numInputs;
    }
    int calcPos(int s, int i) const {
      return s * numInputs + i;
    }

    bool isQE(int si, int sj) const {
      map<pair<int, int>, bool>::const_iterator eqp = qeRel.find(make_pair(si, sj));
      if (eqp != qeRel.end()) {
        return (*eqp).second;
      } else {
        return false;
      }
    }

    const vector<int> &getGamma(int si, int sj) const {
      static vector<int> emptySeq;
      if (si > sj) {
        int temp = si;
        si = sj;
        sj = temp;
      }
      map<pair<int, int>, vector<int> >::const_iterator g = gamma.find(make_pair(si, sj));
      if (g != gamma.end()) {
//        cout << "42" << endl;
        return (*g).second;
      } else {
//        cout << "45" << endl;
        return emptySeq;
      }
    }
    int getNumStates() const {
      return numStates;
    }
    int getNumInputs() const {
      return numInputs;
    }
    int getNumOutputs() const {
      return numOutputs;
    }
    int getNumTransitions() const {
      int numTrans = 0;
      for(int i = 0; i < numStates*numInputs; i++) {
       if (nextState[i] != -1) {
          numTrans++;
       }
      }
      return numTrans;
    }

    int initialState;
    map<string, int> stateToInt;
    map<int, string> intToState;
    int numStates;
    map<string, int> inputToInt;
    map<int, string> intToInput;
    int numInputs;
    map<string, int> outputToInt;
    map<int, string> intToOutput;
    int numOutputs;
    struct TempTrans {
      int si, in, out, sj;
    };
    vector<TempTrans> tempTransitions;
    vector<pair<int, int> > quasiEq;
    int *nextState;
    int *output;
    map<pair<int, int>, bool> qeRel;
    set<int> coreStates;
    set<vector<int> > coreCover;
    map<int, vector<int> > stateCover;
    map<pair<int, int>, vector<int> > gamma;
    set<set<int> > R;
    set<int> states;
  private:
};

#endif
