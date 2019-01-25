//
// Copyright 2009 Konstantinos V. Katsaros
//                              ntinos@aueb.gr
//                              http://mm.aueb.gr/~katsaros
//

// This file is part of BitTorrent Implementation for OMNeT++.

//    BitTorrent Implementation for OMNeT++ is free software: you can redistribute
//	  it and/or modify it under the terms of the GNU General Public License as
//    published by the Free Software Foundation, either version 2 of the License,
//	  or (at your option) any later version.

//    BitTorrent Implementation for OMNeT++ is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License
//    along with BitTorrent Implementation for OMNeT++.
//    If not, see <http://www.gnu.org/licenses/>.


#ifndef __BTStatistics_H__
#define __BTStatistics_H__


#include <omnetpp.h>
#include "BTStatisticsMsg_m.h"

# define BT_STATS_DWL			59760
# define BT_STATS_PP			59761
# define BT_STATS_NSB			59762
# define BT_STATS_EXIT			59763
# define BT_STATS_STIME			59764
# define BT_STATS_DRATE			59765
# define BT_STATS_URATE			59766

# define BT_STATS_MSG_TIME 		20000

using namespace std;

/**
 * Module that calculates the multicast groups sizes and randomly selects their participants (receivers and sender).
 * It is also used for global statistics measurement such as stretch.
 *
 * @author Konstantinos Katsaros
 */

class INET_API BTStatistics : public cSimpleModule
{
	public:
		/**
		* Destructor
		*/
		~BTStatistics();		

	protected:
		int currentTerminalNum;
		int targetOverlayTerminalNum;

		cStdDev* dwSuccess;
		cOutVector dwSuccess_vec;

		cStdDev* numBlockFail;
		cOutVector numBlockFail_vec;

		cStdDev* dataProviders;
		cOutVector dataProviders_vec;

		cStdDev* numSeederBlocks;
		cOutVector numSeederBlocks_vec;

		cStdDev* startupTime;
		cOutVector startupTime_vec;

		cStdDev* downloadRate;
		cOutVector downloadRate_vec;

		cStdDev* uploadRate;
		cOutVector uploadRate_vec;
		
		/**
		* Check whether the statistics collection has completed
		*/
		void checkFinish();

		/**
		* Init member function of module
		*/
		virtual void initialize();

		/**
		* HandleMessage member function of module
		*/
		virtual void handleMessage(cMessage* msg);

		/**
		* Finish member function of module
		*/
		virtual void finish();

		/**
		* Do the actual finish() call and record scalars
		*/
		void doFinish();
};

#endif
