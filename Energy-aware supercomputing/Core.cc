#include "Core.h"

using namespace std;

Core::Core(){

}

/**
 * Computes the current power consumption of the cores based on the technology (numberOfCores) and the Operating system
 *
 * @param Architecture (AMD/Intel), operatingSystem (Linux/Windows), numberOfCores
 * @return double value containing information on power consumption of the cores
 */

double Core::computePower(int Architecture, int operatingSystem, int numberOfCores)
{

	double corePower = 0;
    double fg = Frequency;
    double V = Voltage;

    if(Architecture == 1 && numberOfCores == 4)
    {
    	corePower = (V*3.052 + 1.5 * (fg - 2.0)) * fg * V * V;
    }
    else
    {
    	corePower = computeCapacitance(fg, V, numberOfCores) * fg * V * V;
    }

    // For windows operating systems on Intel CPUs
    if(Architecture == 0 && operatingSystem == 1)
    {
    	corePower = corePower * 0.95;
    }
    else if(Architecture == 1 && operatingSystem == 1)
    {
    	corePower = corePower * 0.93;
    }

    return corePower;
}

/**
 * Computes the capacitance of the power consumption for cores
 *
 * @param frequency, voltage and numberOfCores
 * @return double value containing information on power consumption of the capacitance of the cores
 */

double Core::computeCapacitance(double frequency, double voltage, int numberOfCores)
{
	double factor1 = voltage * 2.43; // Indicates reference value for 2.0 ghz cpus
	double factor2 = 0.0; // Indicates rate of change in capacitance for processor with frequency higher than 2.0 ghz

	if(numberOfCores == 1)
	{
		factor2 = 2.16;
	}
	else
	{
		if(voltage == 1.325)
		{
			factor2 = 1.5;
		}
		else if(voltage == 1.1)
		{
			factor2 = 7.05;
		}
		else
		{
			factor2 = (1.5+7.05)/2;
		}
	}

	return factor1+factor2*(frequency-2.0);
}
