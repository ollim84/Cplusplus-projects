#ifndef __INET_HARDDISK_H
#define __INET_HARDDISK_H

#include <omnetpp.h>

//using namespace std;

class HardDisk { // class name
 public:
  HardDisk();// {} // default constructor

  double P_idle;
  double P_stdby;
  double P_loaded;
  double P_startup;
  double t_startup;
  double loadedTime; //time the jobs spends in a loaded state
  int HDDstate; //Hard Disk state: 1 - idle, 2 - loaded, 3 - stdby

  double computeEnergy(simtime_t from, simtime_t to);
  double computeStartUpPower();

 private:
};

#endif
