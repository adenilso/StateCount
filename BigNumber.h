#ifndef __BIGNUMBER_H__
#define __BIGNUMBER_H__

#include <ostream>

#define MAXVAL 10000
#define NUMPARTS 30

class BigNumber {
public:
  BigNumber() {
    for (int i = 0; i < NUMPARTS; i++) {
      parts[i] = 0;
    }
  }

  BigNumber(long v) {
    for (int i = 0; i < NUMPARTS; i++) {
      parts[i] = 0;
    }
    parts[0] = v;
    arrange();
  }

  BigNumber(const BigNumber &bn) {
    for (int i = 0; i < NUMPARTS; i++) {
      parts[i] = bn.parts[i];
    }
  }

  BigNumber operator =(const BigNumber &bn) {
    for (int i = 0; i < NUMPARTS; i++) {
      parts[i] = bn.parts[i];
    }
    return *this;
  }

  BigNumber shiftLeft(int n = 1) {
    BigNumber bs;
    for (int i = 0; i < NUMPARTS - n - 1; i++) {
      bs.parts[i+n] = parts[i];
    }
    return bs;
  }

  BigNumber shiftRight(int n = 1) {
    BigNumber bs;
    for (int i = 0; i < NUMPARTS - n - 1; i++) {
      bs.parts[i] = parts[i+n];
    }
    return bs;
  }

  operator long() {
    return parts[1]*MAXVAL+parts[0];
  }

  BigNumber operator++(int) {
    parts[0]++;
    arrange();
    return *this;
  }

  void arrange();
  friend std::ostream & operator << (std::ostream &os, const BigNumber &bn);
  friend std::istream & operator >> (std::istream &is, BigNumber &bn);
  friend bool operator == (const BigNumber &bn1, const BigNumber &bn2);
  friend bool operator != (const BigNumber &bn1, const BigNumber &bn2);
  friend bool operator > (const BigNumber &bn1, const BigNumber &bn2);
  friend bool operator >= (const BigNumber &bn1, const BigNumber &bn2);
  friend bool operator < (const BigNumber &bn1, const BigNumber &bn2);
  friend bool operator <= (const BigNumber &bn1, const BigNumber &bn2);
  
  BigNumber operator += (const BigNumber &bn2);

  friend BigNumber operator + (const BigNumber &bn1, const BigNumber &bn2);
  friend BigNumber operator - (const BigNumber &bn1, const BigNumber &bn2);
  friend BigNumber operator * (const BigNumber &bn1, const BigNumber &bn2);
  friend BigNumber operator + (const BigNumber &bn1, long v);
  friend BigNumber operator - (const BigNumber &bn1, long v);
  friend BigNumber operator * (const BigNumber &bn1, long v);
  friend BigNumber operator / (const BigNumber &bn1, long v);
  friend BigNumber operator % (const BigNumber &bn1, const BigNumber &bn2);
  friend BigNumber operator / (const BigNumber &bn1, const BigNumber &bn2);


  friend BigNumber Power(const BigNumber &bn1, long e);
  friend BigNumber Fatorial(long n);

private:
  long parts[NUMPARTS];
};

double bnPercentage(const BigNumber &bn1, const BigNumber &bn2);

#endif
