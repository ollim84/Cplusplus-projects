#include "RAMstick.h"

RAMstick::RAMstick(){
 // default constructor
}

//int Size;
//int type; //2=DDR2, 3=DDR3, 4=Other
//bool buffered; //true = buffered, false = unbuffered
// int Vendor; //==Vendor, 0=Kingston, 1 = Samsung, 2=Hynix, 3=generic
//double Voltage;

double RAMstick::computeEnergy(simtime_t from, simtime_t to){

  double E_idle = 0.0;
 // double E_loaded = 0.0;
  //double P_idle = 0.0;
  //double P_loaded = 0.0;
  double DDR = 0.0;
  double beta = 7.347;

  // DDR2
  if(Type == 2){

	  // Idle power for different vendors
	  if(Vendor == 0) // Kingston
	  {
		  E_idle = Frequency * 0.001 * Size * 0.001;
	  }
	  else if (Vendor == 1) // Samsung
	  {
		  E_idle = 0.95 * Frequency * 0.001 * Size  * 0.001;
	  }
	  else if (Vendor == 2) //Hynix
	  {
		  E_idle = 1.9 * Frequency * 0.001 * Size * 0.001;
	  }
	  else // Generic (rough approximation)
	  {
		  E_idle = 1.45 * Frequency * 0.001 * Size * 0.001;
	  }

	  // Loaded power for unbuffered and buffered RAMsticks
	  if(Loaded)
	  {
		  if(!Buffered)
		  {
			  E_idle = E_idle + beta;
		  }
		  else
		  {
			  if(Vendor == 0) // Kingston
			  {
				  E_idle = 2.2 * E_idle + 2.3 * beta;
			  }
			  else if(Vendor == 1) // Samsung
			  {
				  E_idle = 4.26 * E_idle + 2.3 * beta;
			  }
			  else if(Vendor == 2) // Hynix
			  {
				  E_idle = 1.65 * E_idle + 2.3 * beta;
			  }
			  else // Generic
			  {
				  E_idle = 2.7 * E_idle + 2.3 * beta;
			  }
		  }
	  }

  }//type2

  //DDR3
  else if(Type == 3){

    const int fc = 1600;
    double alpha = 0.000026;
    DDR = Frequency * 0.001 + alpha * sqrt(Frequency) * (fc - Frequency);
    E_idle = Size * 0.001 * DDR;

    if(Loaded)
    {
    	if(!Buffered)
    	{
    		E_idle = E_idle + 1.3 * beta;
    	}
    	else //buffered (rough approximation)
    	{
    		E_idle = 2.0 * E_idle + 1.9 * beta;
    	}
    }

  }
  // Other type (rough approximation)
  else
  {
	  E_idle = 2.7;
	  if(Loaded)
	  {
		  E_idle = E_idle + beta;
	  }
  }

  return E_idle * SIMTIME_DBL(to-from);

}

double RAMstick::getIdlePower()
{

  double E_idle = 0.0;
  double DDR = 0.0;

  // DDR2
  if(Type == 2){

	  // Idle power for different vendors
	  if(Vendor == 0) // Kingston
	  {
		  E_idle = Frequency * 0.001 * Size * 0.001;
	  }
	  else if (Vendor == 1) // Samsung
	  {
		  E_idle = 0.95 * Frequency * 0.001 * Size * 0.001;
	  }
	  else if (Vendor == 2) //Hynix
	  {
		  E_idle = 1.9 * Frequency * 0.001 * Size * 0.001;
	  }
	  else // Generic (rough approximation)
	  {
		  E_idle = 1.45 * Frequency * 0.001 * Size * 0.001;
	  }
  }

  //DDR3
  else if(Type == 3){

    const int fc = 1600;
    double alpha = 0.000026;
    DDR = Frequency * 0.001 + alpha * sqrt(Frequency) * (fc - Frequency);
    E_idle = Size * 0.001 * DDR;

  }
  // Other type (rough approximation)
  else
  {
	  E_idle = 2.7;
  }

  return E_idle;

}
