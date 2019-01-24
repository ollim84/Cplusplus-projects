#ifndef __INET_PSU_H
#define __INET_PSU_H

#include <omnetpp.h>

class PSU{ // class name
 public:
  PSU();// default constructor

  double computeEnergy(simtime_t from, simtime_t to);
  double computePSUEnergy(simtime_t from, simtime_t to, int numPSU, double serverPower);

  double PSUpower; //power of the PSU
  double PSUefficiency; //efficiency of the PSU


 private:
};

#endif
