#ifndef __INET_NIC_H
#define __INET_NIC_H

#include <omnetpp.h>
#include <InterfaceTable.h>
//#include <PPP.h>
#include <EtherMACBase.h>
#include <EtherMAC.h>

class NIC{ // class name
 public:
  NIC();// default constructor

  double computeEnergy(simtime_t from, simtime_t to, cModule *module);

  double P_idle; //idle power
  double P_max; //max power

  // PPP interface
  cModule *module;
  EtherMAC *ethMac;

  // Sent/Rcvd packets
  long PrevRcvd;
  long PrevSent;
  long CurRcvd;
  long CurSent;
  long Rcvd;
  long Sent;
  double pps; //packets per second


 private:
};

#endif
