#include "Fan.h"


using namespace std;

Fan::Fan(){}
/*
  int Max_RPM;
  int Actual_RPM;
  double Power_idle;
  double Power_max;
  int Width;
  int Depth;
*/
double Fan::computeEnergy(simtime_t from, simtime_t to)
{

  double E = 0.0;
  double P = 0.0;
 // const int c = 40;

  // Formula in the deliverable
  //E = pow(Actual_RPM, 2.4) * pow(Width,2.0) * sqrt(Depth) /(c *Max_RPM);

//  E = 8.33068 * pow(10,-15) * pow(Actual_RPM,4) + 8.51757 * pow(Width*0.001,4) - 2.9569 * pow(Depth*0.001,4)
//  - 1.10138 * pow(10,-10) * pow(Actual_RPM,3) + 54.6855 * pow(Width*0.001,3) - 76.4897 * pow(Depth*0.001,3)
//  + 4.85429 * pow(10,-7) * pow(Actual_RPM,2) + 258.847 * pow(Width*0.001,2) - 1059.02 * pow(Depth*0.001,2)
//  - 6.06127 * pow(10,-5) * Actual_RPM + 32.6862 * Width * 0.001 + 67.3012 * Depth * 0.001 - 5.478;

  // Power consumed by the FAN (new formula)
  P = 8.33068 * pow(10,-15) * pow(Actual_RPM,4) + 8.51757 * pow(Width,4) - 2.9569 * pow(Depth,4)
  - 1.10138 * pow(10,-10) * pow(Actual_RPM,3) + 54.6855 * pow(Width,3) - 76.4897 * pow(Depth,3)
  + 4.85429 * pow(10,-7) * pow(Actual_RPM,2) + 258.847 * pow(Width*0.001,2) - 1059.02 * pow(Depth,2)
  - 6.06127 * pow(10,-5) * Actual_RPM + 32.6862 * Width + 67.3012 * Depth - 5.478;

  //This provides non-negative numbers when unsupported (?) Fans are being used
  //P = max(1.2, P);

  // Energy
  E =  P * SIMTIME_DBL(to-from);


  return E;

}

double Fan::getIdlePower()
{

  double P = 0.0;
  //const int c = 40;
  //E = pow(Actual_RPM, 2.4) * pow(Width,2.0) * sqrt(Depth) /(c *Max_RPM);
  P = 8.33068 * pow(10,-15) * pow(Actual_RPM,4) + 8.51757 * pow(Width,4) - 2.9569 * pow(Depth,4)
  - 1.10138 * pow(10,-10) * pow(Actual_RPM,3) + 54.6855 * pow(Width,3) - 76.4897 * pow(Depth,3)
  + 4.85429 * pow(10,-7) * pow(Actual_RPM,2) + 258.847 * pow(Width*0.001,2) - 1059.02 * pow(Depth,2)
  - 6.06127 * pow(10,-5) * Actual_RPM + 32.6862 * Width + 67.3012 * Depth - 5.478;
  return P;

}
