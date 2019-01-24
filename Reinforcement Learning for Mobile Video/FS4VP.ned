//
// Copyright 2011 Zarrar Yousaf (NEC)
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//


package inet.applications.tcpapp;

//
// Fair Scheduler for Video Pacing. Designed specifically for progressively downloaded TCP video streams/content 
//
simple FS4VP
{
    parameters:
        int numTcpApps = default(0);
        double txRate = default(0); //specifies the rate at which an app is allowed to tx content 
        int bytesToSend @unit("B") = default(1MB); //indicates the total amount of content data in bytes.
        int chunkSize @unit("B") = default(100B); //divides the bytestToSend into chunks, where the size of the chunk is specified by chunkSize
        double iatChunk = default(0); //specifies the inter-tx time of content chunks (i.e., inter-arrival time (iat) between chunks).
        							  // Default is zero, meaning that the content is sent as one big chunk of size bytesToSend.
    	bool bufferEstimation = default(true); //enable/disable the FS4VP to perform buffer estimation
    	double alpha; //its a weight given to the application tranmission rate 
    	int maxSegSize; //defined as static here, TODO MUST be attained from the TCP layer.
    	// RL agent
    	double agentControlTime @unit("s") = default(1s); // Control time of the agent
    	double alphaRL = default(0.5); // learning rate
    	double gamma = default(0.8); // discount factor
    	double beeta = default(0.8); // temperature parameter
    	bool learningEnabled = default(false); // whether learning is enabled
    	bool burst = default(false); // whether a temporary burst for increase/decrease is used
    	// Coefficients for reward function
    	double uN1Gold = default(0);
		double uN2Gold = default(0);
		double uN3Gold = default(0);
		double uN1Silver = default(0);
		double uN2Silver = default(0);
		double uN3Silver = default(0);
		double uN1Bronze = default(0);
		double uN2Bronze = default(0);
		double uN3Bronze = default(0);
		// Increase/decrease percentages
		double goldIncPercentage = default(0);
		double silverIncPercentage = default(0);
		double bronzeIncPercentage = default(0);
		double goldDecPercentage = default(0);
		double silverDecPercentage = default(0);
		double bronzeDecPercentage = default(0);
		
		bool changeChunksize = default(true); // TRUE: chunksize is changed, FALSE: txRate is changed
		bool readQmatrixFromFile = default(false); // Whether we want to read the Q-matrix from a file
		bool writeQmatrixToFile = default(false);  // Whether we want to write the Q-matrix into a file
		bool initialPolicy = default(false); //whether we use initial policy
		bool scheduleRestart = default(false); // used for starting content sending again to gold user after idle period
		bool randomQTable = default(false); // Whether random Q table is used
		double idlePeriod @unit("s") = default(0s);
        @display("i=block/classifier");
    gates:
        input directInput @directIn; //interface to receive sendDirect Message from apps

}

