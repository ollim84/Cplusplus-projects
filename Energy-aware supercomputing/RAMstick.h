#ifndef __INET_RAMSTICK_H
#define __INET_RAMSTICK_H

#include <omnetpp.h>

class RAMstick { // class name
 public:
  RAMstick();// {} // default constructor
  //CPU(int n); // constructor overloaded

  double Size;
  int Vendor; //0=Kingston, 1 = Samsung, 2=Hynix
  double Voltage;
  int Type; //2=DDR2, 3=DDR3
  bool Buffered; //true = buffered, false = unbuffered
  int Frequency; //input freq in MHz
  bool Loaded; // under load = true,  idle = false

  double computeEnergy(simtime_t from, simtime_t to);
  double getIdlePower();

 private:
};

#endif
