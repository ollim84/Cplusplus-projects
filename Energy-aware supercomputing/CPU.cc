#include "CPU.h"
//#include <map>

//Define_Module(CPU);
using namespace std;

CPU::CPU(){
  //do we need that?

}

double CPU::computePower(simtime_t from, simtime_t to)
{
	double E = 0.0; //Total consumed energy
	double E_idle = Power_idle; //Consumed energy in idle state

//	double alpha;
//	double beta;
//	double fmin = 2.0; //2.0 GHz reference frequency
//	double fg = 2.4; //TODO: make parameter
//	double C; //Effective switched capacitance
//	double V; //Voltage
//	double L; //Percentage Load
	double corePower = 0.0;
	double cpuPower = 0.0;
	int loadedCoreCount = 0;
	double maximumFrequency = 2.0; //Represents the maximum frequency with which any core of the same CPU is running
	double idleMinFrequency = 2.0;
	double coreFrequency;

	SameFreqCount sameFreqCount;
	SameFreqPower sameFreqPower;

	// Architecture enumeration, 0=Intel, 1 = AMD
	// operatingSystem enumeration 0 = Linux, 1 = Windows

	// Go through the cores and calculate power of each core and add it to the CPU power
	for(int i=0;i<(int)cores.size();i++){ //could use also corenumber

		if(cores[i].Frequency > maximumFrequency)
			maximumFrequency = cores[i].Frequency;

		corePower = cores[i].computePower(Architecture, operatingSystem, (int)cores.size()) * cores[i].Load * 0.01;
		cpuPower = cpuPower + corePower; //TODO: calculate power if core load is 0.5?

		if(cores[i].Load > 0.5) // core is loaded, i.e. running a job
		{
			loadedCoreCount++;
		}

		coreFrequency = cores[i].Frequency;

		SameFreqCount::iterator i;
		i = sameFreqCount.find(coreFrequency);

	    if(i != sameFreqCount.end())
	    {
	    	sameFreqCount.insert(std::pair<double,int>(coreFrequency, (i->second) + 1));
	    	sameFreqPower.insert(std::pair<double,double>(coreFrequency, (i->second) + corePower));
	    }
	    else
	    {
	    	sameFreqCount.insert(std::pair<double,int>(coreFrequency, 1));
	    	sameFreqPower.insert(std::pair<double,double>(coreFrequency, corePower));
	    }


	}

	/**
	 * The idle power (with respect to the change of frequency) on Intel machines is same for different
	 * frequencies while on AMD, each frequency has different idle power.
	 * Here min_freq indicates the frequency when the core/cpu is in idle mode
	 */

	if(loadedCoreCount > 0)
	{
		//Quad core processors
		if((int)cores.size() == 4 && Architecture == 1)
		{
			if(maximumFrequency > idleMinFrequency)
			{
				cpuPower = cpuPower + 18 * (maximumFrequency - idleMinFrequency);
			}
		}
		else
		{
			// On intel machine, there is a need to subtract the extra power from summation
			// of power consumption
			if(loadedCoreCount > 1)
			{
			    for (SameFreqCount::iterator i=sameFreqCount.begin(); i!=sameFreqCount.end(); ++i)
			    {
			    	if(i->second > 1)
			    	{
			    		SameFreqPower::iterator iter;
			    		iter = sameFreqPower.find(i->first);
			    		cpuPower = cpuPower - calEnergRedFact((int)cores.size(), i->first , i->second) * (iter->second);
			    	}
			    }
			}

			// Since AMD consumes near about the same power as Intel, therefore, we calculate
			// power for Intel and take into account for AMD
			if(Architecture == 1)
			{
				cpuPower = cpuPower * 1.02;
			}

		}
	}


	E = cpuPower;

	E += E_idle;
	//multiply E with the time elapsed
	E = E * SIMTIME_DBL(to-from);
	return E;

}

/**
 * This function is specific for Intel machines because the power consumption
 * of N cores is less than the sum of power consumptions of these N cores.
 * This method calculates the error factor, which then will be subtracted from
 * the summation to obtain the actual power consumption.
 */

double CPU::calEnergRedFact(int numberOfCores, double frequency, int loadedCoreCount)
{
	double alpha = 0.0; // base value for 2.0 ghz frequency
	double beta = 0.0; // reflect the error due to change in frequency
	double gamma = 0.0; // shows the rate of change with number of cores

	if(numberOfCores == 2) // dual core
	{
		return 0.01;
	}
	else
	{
		alpha = 0.04;
		beta = 0.42;
		gamma = 0.015;
	}
	return alpha + beta * (frequency - 2.0) + gamma * (loadedCoreCount - 2);
}

double CPU::getIdlePower()
{

  double E = 0.0; //Total consumed energy
  double E_idle = Power_idle; //Consumed energy in idle state
  double L; //Percentage Load
  double corePower = 0.0;
  double cpuPower = 0.0;

  L = 0.5;

	// Architecture enumeration, 0=Intel, 1 = AMD
	// operatingSystem enumeration 0 = Linux, 1 = Windows

	// Go through the cores and calculate power of each core and add it to the CPU power
	for(int i=0;i<(int)cores.size();i++){ //could use also corenumber

		corePower = cores[i].computePower(Architecture, operatingSystem, (int)cores.size()) * L * 0.01;
		cpuPower = cpuPower + corePower; //TODO: calculate power if core load is 0.5?

	}

  E += E_idle;

  return E;

}

//double CPU::computePower(simtime_t from, simtime_t to){
//
//  double E = 0.0; //Total consumed energy
//  double E_idle = Power_idle; //Consumed energy in idle state
//
//  double alpha;
//  double beta;
//  double fmin = 2.0; //2.0 GHz reference frequency
//  double fg = 2.4; //TODO: make parameter
//  double C; //Effective switched capacitance
//  double V; //Voltage
//  double L; //Percentage Load
//
//
//  for(int i=0;i<(int)cores.size();i++){ //could use also corenumber
//
//    fg = cores[i].Frequency;
//    L = cores[i].Load;
//
//    if(cores.size()==2)
//    {
//    	alpha = 0.134;
//    	beta = 0.156;
//    }
//    else if(cores.size() == 4){
//
//    	if(Architecture == 3) //AMD + Linux
//    	{
//    		alpha = 0.202;
//			beta = 0.31;
//    	}
//    	else if(Architecture == 4) //AMD + Windows
//    	{
//    		alpha = 0.0828;
//			beta = 0.044;
//    	}
//    	else //Linux+Intel and Windows + Intel
//    	{
//    		alpha = 0.111;
//    		beta = 0.105;
//    	}
//
//    }
//    else if(cores.size()==8){
//      alpha = 0.090;
//      beta = 0.168;
//    }
//
//    if(Architecture == 1) //Intel+Windows
//    {
//    	if(cores.size()==2)
//    	{
//    		C = alpha + beta*(fg-fmin);
//    	}
//    	else if(cores.size()==4)
//    	{
//    		C = alpha + beta*(fg-fmin)*0.95;
//    	}
//    	else if(cores.size()==8)
//		{
//			C = alpha + beta*(fg-fmin)*0.93;
//		}
//    }
//    else //Intel+Linux, AMD+Linux and AMD+Windows
//    {
//    	C = alpha + beta*(fg-fmin);
//    }
//
//
//    V = cores[i].Voltage * cores[i].Voltage; //Vi^2
//    L = cores[i].Load;
//
//    E += C*V*L;
//
//    //Intel+Linux and Intel+Windows
//    if(Architecture == 0 || Architecture == 1)
//    {
//
//		if(L > 50.0){ //if load > 50%
//		  if(cores.size()==2)
//			  C = C*0.423;
//		  else if(cores.size()==4)
//			  C = C*0.42;
//		  else if(cores.size()==8)
//			  C = C*0.4;
//
//		  L = L-50.0;
//
//		  E += C*V*L;
//		}
//    }
//    //AMD+Linux and AMD+Windows
//    else
//    {
//    	if(L > 25.0)
//    	{
//    		if(cores.size()==4)
//    		{
//    			if(Architecture == 3) //AMD + Linux
//    			{
//    				alpha = 0.084;
//					beta = 0.13;
//    			}
//    			else if(Architecture == 4) //AMD + Windows
//    			{
//    				alpha = 0.0828;
//					beta = 0.044;
//    			}
//
//    			C = alpha + beta*(fg-fmin);
//    			L = L-25.0;
//    			E += C*V*L;
//    		}
//
//    	}
//
//    }
//
//
//  }//for(i...
//
//  E += E_idle;
//  //multiply E with the time elapsed
//  E = E * SIMTIME_DBL(to-from);
//  return E;
//
//}

//double CPU::getIdlePower()
//{
//
//  double E = 0.0; //Total consumed energy
//  double E_idle = Power_idle; //Consumed energy in idle state
//
//  double alpha;
//  double beta;
//  double fmin = 2.0; //2.0 GHz reference frequency
//  double fg = 2.4; //TODO: make parameter
//  double C; //Effective switched capacitance
//  double V; //Voltage
//  double L; //Percentage Load
//
//
//  //FOR INTEL+LINUX ARCHITECTURE !!!!
//  for(int i=0;i<(int)cores.size();i++){ //could use also corenumber
//    fg = cores[i].Frequency;
//    L = 0.5; //Set the load what the OS has
//    if(cores.size()==2){
//    	alpha = 0.134;
//    	beta = 0.156;
//    }
//    else if(cores.size() == 4){
//      alpha = 0.111;
//      beta = 0.105;
//    }
//    else if(cores.size()==8){
//      alpha = 0.090;
//      beta = 0.168;
//    }
//
//    C = alpha + beta*(fg-fmin);
//    V = cores[i].Voltage * cores[i].Voltage; //Vi^2
//    L = 0.5;
//
//    E += C*V*L;
//
//  }//for(i...
//
//  E += E_idle;
//  //multiply E with the time elapsed
//  //E = E * SIMTIME_DBL(to-from);
//  return E;
//
//}

