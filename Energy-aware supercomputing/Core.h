#ifndef __INET_CORE_H
#define __INET_CORE_H


class Core { // class name
 public:
  Core();// default constructor

  double computePower(int Architecture, int operatingSystem, int numberOfCores);
  double computeCapacitance(double frequency, double voltage, int numberOfCores);

  double Frequency;
  double Voltage;
  double Load;

 private:
};

#endif
