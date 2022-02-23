
#include <iostream>
#include "BigNumber.h"

void BigNumber::arrange() {
  int i = 0;
  while(i < NUMPARTS) {
    long add = parts[i] / MAXVAL;
    parts[i] = parts[i] % MAXVAL;
    parts[i+1] += add;
    i++;
  }
}

std::ostream & operator << (std::ostream &os, const BigNumber &bn) {
  int first = true;
  for (int i = NUMPARTS - 1; i >= 0; i--) {
    if (first) {
      if (bn.parts[i] > 0) {
        first = false;
        os << bn.parts[i];
        //os << '.';
      }
    } else {
      long val = MAXVAL / 10;
      while (val > bn.parts[i]) {
        os << "0";
        val /= 10;
      }
      if (bn.parts[i] > 0) {
        os << bn.parts[i];
      }
      //os << '.';
    }
  }
  if (first) {
    os << 0;
  }
  return os;
}

std::istream & operator >> (std::istream &is, BigNumber &bn) {
  bn = 0;
  BigNumber ten = 10;
  char c;
  while((c = is.get()) != EOF) {
    if (c >= '0' && c <= '9') {
      int val = c - '0';
       bn = ten*bn + BigNumber(val);
    } else {
      break;
    }
  }
  return is;
}

bool operator == (const BigNumber &bn1, const BigNumber &bn2) {
  for (int i = NUMPARTS - 1; i >= 0; i--) {
    if (bn1.parts[i] != bn2.parts[i]) {
      return false;
    }
  }
  return true;
}

bool operator != (const BigNumber &bn1, const BigNumber &bn2) {
  for (int i = NUMPARTS - 1; i >= 0; i--) {
    if (bn1.parts[i] != bn2.parts[i]) {
      return true;
    }
  }
  return false;
}

bool operator > (const BigNumber &bn1, const BigNumber &bn2) {
  for (int i = NUMPARTS - 1; i >= 0; i--) {
    if (bn1.parts[i] > bn2.parts[i]) {
      return true;
    } else if (bn1.parts[i] < bn2.parts[i]) {
      return false;
    }
  }
  return false;
}

bool operator >= (const BigNumber &bn1, const BigNumber &bn2) {
  for (int i = NUMPARTS - 1; i >= 0; i--) {
    if (bn1.parts[i] > bn2.parts[i]) {
      return true;
    } else if (bn1.parts[i] < bn2.parts[i]) {
      return false;
    }
  }
  return true;
}

bool operator < (const BigNumber &bn1, const BigNumber &bn2) {
  for (int i = NUMPARTS - 1; i >= 0; i--) {
    if (bn1.parts[i] < bn2.parts[i]) {
      return true;
    } else if (bn1.parts[i] > bn2.parts[i]) {
      return false;
    }
  }
  return false;
}

bool operator <= (const BigNumber &bn1, const BigNumber &bn2) {
  for (int i = NUMPARTS - 1; i >= 0; i--) {
    if (bn1.parts[i] < bn2.parts[i]) {
      return true;
    } else if (bn1.parts[i] > bn2.parts[i]) {
      return false;
    }
  }
  return true;
}

BigNumber BigNumber::operator += (const BigNumber &bn2) {
  for (int i = 0; i < NUMPARTS; i++) {
    parts[i] += bn2.parts[i];
  }
  arrange();
  return *this;
}

BigNumber operator + (const BigNumber &bn1, const BigNumber &bn2) {
  BigNumber bs;
  for (int i = 0; i < NUMPARTS; i++) {
    bs.parts[i] = bn1.parts[i] + bn2.parts[i];
  }
  bs.arrange();
  return bs;
}

BigNumber operator + (const BigNumber &bn1, long v) {
  BigNumber bs = bn1;
  bs.parts[0] += v;
  bs.arrange();
  return bs;
}

BigNumber operator - (const BigNumber &bn1, const BigNumber &bn2) {
  BigNumber bs;
  if (bn1 > bn2) {
    bs = bn1;
    for (int i = 0; i < NUMPARTS; i++) {
      if (bs.parts[i] < bn2.parts[i]) {
        bs.parts[i] += MAXVAL;
        bs.parts[i+1]--;
      }
      bs.parts[i] -= bn2.parts[i];
    }
    bs.arrange();
  }
  return bs;
}

BigNumber operator - (const BigNumber &bn1, long v) {
  return bn1 - BigNumber(v);
}

BigNumber operator * (const BigNumber &bn1, const BigNumber &bn2) {
  BigNumber bs;
  for (int i = 0; i < NUMPARTS; i++) {
    for (int j = 0; j + i < NUMPARTS; j++) {
      bs.parts[i+j] += bn1.parts[i] * bn2.parts[j];
    }
  }
  bs.arrange();
  return bs;
}

BigNumber operator * (const BigNumber &bn1, long v) {
  BigNumber bs;
  for (int i = 0; i < NUMPARTS; i++) {
    bs.parts[i] += bn1.parts[i] * v;
  }
  bs.arrange();
  return bs;
}

BigNumber operator / (const BigNumber &bn1, long v) {
  BigNumber bs = bn1;
  for (int i = NUMPARTS - 1; i >= 0; i--) {
    long aux = bs.parts[i] % v;
    bs.parts[i] /= v;
    if (i > 0) {
      bs.parts[i-1] += aux * MAXVAL;
    }
  }
  bs.arrange();
  return bs;
}

void bnDivMod(const BigNumber &bn1, const BigNumber &bn2, BigNumber &q, BigNumber &r) {
  q = 0;
  r = bn1;                                                                             
  if (bn2 != BigNumber(0)) {
    BigNumber div = bn2;
    BigNumber aux = div.shiftLeft(1);
    //std::cout << bn1 << " / " << bn2 << std::endl;
    //std::cout << "aux = " << aux << std::endl;
    while (aux <= bn1) {
      div = aux;
      aux = aux.shiftLeft(1);
      //std::cout << "aux = " << aux << std::endl;
      //std::cout << "bn1 = " << bn1 << std::endl;
    }
    aux = div * BigNumber(10);
    while (aux <= bn1) {
      div = aux;
      aux = aux * BigNumber(10);
      //std::cout << "aux = " << aux << std::endl;
      //std::cout << "bn1 = " << bn1 << std::endl;
    }
    while (div >= bn2) {
      long v = 0;
      while (div * BigNumber(v + 1) <= r) {
        v++;
      }
      q = q * BigNumber(10);
      q = q + BigNumber(v);
      r = r - div * BigNumber(v);
      div = div / long(10);
      //std::cout << "q = " << q << std::endl;
      //std::cout << "v = " << v << std::endl;
      //std::cout << "div = " << div << std::endl;
      //std::cout << "r = " << r << std::endl;
    }
    q.arrange();
    r.arrange();
  }
}

double bnPercentage(const BigNumber &bn1, const BigNumber &bn2) {
//  std::cout << "((BigNumber(100000) * bn1)) = " << ((BigNumber(100000) * bn1)) << std::endl;
//  std::cout << "((BigNumber(100000) * bn1) / bn2)) = " << ((BigNumber(100000) * bn1) / bn2) << std::endl;
  return (double) ((double) ((BigNumber(100000) * bn1) / bn2)) / 1000.0;
}

BigNumber operator / (const BigNumber &bn1, const BigNumber &bn2) {
  BigNumber q, r;
  bnDivMod(bn1, bn2, q, r);
  return q;
}

BigNumber operator % (const BigNumber &bn1, const BigNumber &bn2) {
  BigNumber q, r;
  bnDivMod(bn1, bn2, q, r);
  return r;
}

BigNumber Power(const BigNumber &bn1, long e) {
  BigNumber bs = 1;
  for (long i = 0; i < e; i++) {
    bs = bs * bn1;
  }
  return bs;
}

BigNumber Fatorial(long n) {
  BigNumber bs = 1;
  for (long i = 0; i < n; i++) {
    bs = bs * BigNumber(i);
  }
  return bs;
}

