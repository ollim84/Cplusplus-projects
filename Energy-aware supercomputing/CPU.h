#ifndef __INET_CPU_H
#define __INET_CPU_H

#include <vector>
#include "Core.h"
#include "Cache.h"
#include <omnetpp.h>

class CPU{// : public cSimpleModule{ // class name
 public:
  CPU();// default constructor
  //CPU(int n); // constructor overloaded

  int Architecture; //enumeration, 0=Intel, 1 = AMD
  int operatingSystem; // enumeration 0 = Linux, 1 = Windows
  bool DVFS;
  int Lithography;
  double Power_idle;
  double Power_max;

  //int corenumber; // #cores in this CPU
  std::vector<Core> cores; //container for Core(s)
  std::vector<Cache> caches;//container for Cache(s)

  typedef std::map<double, int> SameFreqCount;
  typedef std::map<double, double> SameFreqPower;

  double computePower(simtime_t from, simtime_t to);
  double calEnergRedFact(int numberOfCores, double frequency, int loadedCoreCount);
  double getIdlePower();

 protected:
  //virtual void initialize();
  //virtual void handleMessage(cMessage *msg);

 private:
};

#endif
