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

#ifndef __STATISTICSCOLLECTOR_H__
#define __STATISTICSCOLLECTOR_H__

#include <omnetpp.h>


/**
 * TODO - Generated class
 */
class StatisticsCollector : public cSimpleModule
{
	public:
		// Desctructor
		~StatisticsCollector();

		void addGoldCompleted();
		void recordStarvationDuration(double, int);
		void recordClientStats(double, double, int, int);
		void addBufferStarvations(int userClass);
		void addCompleted();

	protected:
		virtual void initialize();
		virtual void handleMessage(cMessage *msg);
		virtual void writeQmatrix();
		virtual void finish();

		int numCompleted;
		int numUsers;
		int goldCompleted;
		int numberOfGold;
		cOutVector GoldStarvationDuration;
		cOutVector SilverStarvationDuration;
		cOutVector BronzeStarvationDuration;

		int numberOfGoldStarvations;
		int numberOfSilverStarvations;
		int numberOfBronzeStarvations;

		int statsGoldStarvations;
		int statsSilverStarvations;
		int statsBronzeStarvations;

		std::vector <double> goldDurationVector;
		std::vector <double> silverDurationVector;
		std::vector <double> bronzeDurationVector;

		std::vector <double> goldFreqVector;
		std::vector <double> silverFreqVector;
		std::vector <double> bronzeFreqVector;

		cModule *clientModule;
		cModule *fs4vp;


};

#endif
