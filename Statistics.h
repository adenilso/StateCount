#ifndef __STATISTICS_H__
#define __STATISTICS_H__

#include <values.h>
#include <math.h>

class Statistics {
  public:
    Statistics() {
      min = MAXDOUBLE;
      max = MINDOUBLE;
      sum = 0.0;
      sqrSum = 0.0;
      n = 0;
    }
    void addValue(double val, int freq = 1) {
      if (min > val) {
        min = val;
      }
      if (max < val) {
        max = val;
      }
      sum += val * freq;
      sqrSum += val * val * freq;
      n += freq;
    }
    double getMin() const {
      return min;
    }
    double getMax() const {
      return max;
    }
    double getSum() const {
      return sum;
    }
    int getNum() const {
      return n;
    }
    double getAvg() const {
      if (n > 0) {
        return sum / n;
      } else {
        return 0.0;
      }
    }
    double getSD() const {
      if (n > 1) {
        return sqrt(((n * sqrSum) - (sum * sum)) / (n * (n - 1)));
      } else {
        return 0.0;
      }
    }
  private:
    double min;
    double max;
    double sum;
    double sqrSum;
    int n;
};

#endif
