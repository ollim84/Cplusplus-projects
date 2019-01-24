#ifndef __INET_FAN_H
#define __INET_FAN_H

#include <omnetpp.h>


class Fan { // class name
 public:
  Fan();// default constructor

  int Max_RPM;
  int Actual_RPM;
  double Power_idle;
  double Power_max;
  double Width;
  double Depth;

  double computeEnergy(simtime_t from, simtime_t to);
  double getIdlePower();

 private:
};

#endif
