//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

package inet.applications.supercom;
import inet.applications.tcpapp.TCPApp;

simple SuperComServerApp like TCPApp
{
    parameters:
        string address = default(""); // local address; may be left empty ("")
        int port = default(1000);     // port number to listen on
        double replyDelay @unit("s") = default(0s);
        @display("i=block/app");
        int memory @unit("MB") = default(8GB);
        int numCores = default(1);
        
 		int numRAMsticks = default(4);//number of RAMsticks
    	int RAMstickSize @unit("MB") = default(2GB);//size of each RAMstick in MB???; at first all sticks are of same size! 
    	int numCPUs = default(1);//number of CPUs on this server
    	int numCaches = default(1);
    	int numPSU = default(1);
    	
    	volatile int outputData @unit("B") = default(200B);
    	
    	int server_idle = default(150); //Server idle power in watts
    	int energyMode = default(0); // 0 = Hibernate, 1 = Sleep/Stdby
    	
    	double PSUpower = default(400); //PSUpower, 300-500W ?
    	double PSUefficiency = default(70); //PSU efficiency, typically 70-75%
    	
    	//Network Interface Card
		double NIC_powerIdle = default(1); 
		double NIC_powerMax = default(1);

        //MiM for CPU:
    	int CPU_Architecture = default(1); //enumeration, 0=Intel, 1 = AMD
    	int operatingSystem = default(0); // enumeration 0 = Linux, 1 = Windows
     	bool DVFS = default(false);
      	int Lithography = default(0); //CPU Lithography
      	double CPU_Power_idle = default(15.0);//in W
      	double CPU_Power_max = default(85.0); //in W
    	///////////
    	
    	//OM for Core:
    	double coreFrequency = default(2.4);
    	double coreVoltage = default(1.2);
    	double coreLoad = default(0.5);
    	
    	//OM for RAM
    	int RAMvendor = default(0); //0=Kingston, 1 = Samsung, 2=Hynix
    	double RAMvoltage = default(1.2);
    	int RAMtype = default(2); //2=DDR2, 3=DDR3
    	bool RAMbuffered = default(false); //true = buffered, false = unbuffered
    	double RAMfrequency = default(800); //input freq in MHz
    	bool RAMloaded = default(false);
    	
    	//OM for HDD
    	double HDDidlePower = default(4.0); //4 - 8 W
   	 	double HDDloadedPower = default(5.6); // 1.4 * idlePower  (6 - 12 W)
    	double HDDstdbyPower = default(0.5); //0.13 - 1.0W
    	int HDDstate = default(1); //1 = IDLE, 2 = LOADED, 3 = STDBY/SLEEP, 4 = STARTUP
    	double HDDstartupPower = default(14.8); // 3.7 * idlePower  (16 - 29 W)
    	int HDDstartupTime = default(10); // 10 - 20 s
    	double HDDloadedTime = default(1);
    	
    	// OM for FAN
        int fanMaxRPM = default(12000); //4000 - 16000 rpm
        int fanActualRPM = default(6000); //1000 - 16000 rpm
        double fanPowerIdle = default(0.5);
        double fanPowerMax = default(2.0);
        double fanWidth = default(0.06); //0.04 - 0.12 m
        double fanDepth = default(0.03); //0.01 - 0.04 m
    	
 
 
    	
    gates:
        input tcpIn @labels(TCPCommand/up);
        output tcpOut @labels(TCPCommand/down);
}
