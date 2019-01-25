//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//


import
    "Tiers",
    "Overlay",
    "Router",
    "AccessRouter",
//     "GatewayRouter",
    "WirelessAccessRouter",
    "OverlayRouter",
    "OverlayAccessRouter",
    "TunOutRouter",
    "IPv4UnderlayConfigurator",
    "NullMobility",
    "GlobalObserver",
    "BTStatistics",
    "ChannelControl";
//
// The IPv4Underlay ned-file @see IPv4UnderlayConfigurator
//
module IPv4Underlay

    parameters:
        overlayType: string,    // the overlay used in the simulation (for overlayBackboneRouters)
        tier1Type: string,    // the application on top of the overlay used in the simulation (for overlayBackboneRouters)
        tier2Type: string,    // the module type on top of tier 1 (for overlayBackboneRouters)
        tier3Type: string,    // the module type on top of tier 2 (for overlayBackboneRouters)
        backboneRouterNum: numeric,    // number of backbone routers in the network
 	gatewayRouterNum: numeric,    // number of gateway routers in the network
        accessRouterNum: numeric,    // number of access routers in the network
	wirelessAccessRouterNum: numeric, //number of wireless access routers in the network that do NOT participate the overlay (OM)
        overlayBackboneRouterNum: numeric,    // number of backbone routers in the network that participate the overlay
        overlayAccessRouterNum: numeric,    // number of access routers in the network that participate the overlay
        outRouterNum: numeric,    // set to 1 if you want to use a TunOutRouter as connection to a real network
        connectivity: numeric,    // degree of connectivity of backbone routers (1 = every backbone router is connected to all the others)
	playgroundSizeX: numeric const, //OM
        playgroundSizeY: numeric const; //OM

    submodules:
        underlayConfigurator: IPv4UnderlayConfigurator;
            display: "i=block/cogwheel";
        backboneRouter: Router[backboneRouterNum];
            display: "i=device/server2";
	gatewayRouter: Router[gatewayRouterNum];
            display: "i=device/server2,blue,30";
        outRouter: TunOutRouter[min(1, outRouterNum)];
            display: "i=misc/globe";
        accessRouter: AccessRouter[accessRouterNum];
            display: "i=device/server2,gray,30";
	wirelessAccessRouter: WirelessAccessRouter[wirelessAccessRouterNum]; //OM
            display: "i=device/server2,red,30";
        overlayAccessRouter: OverlayAccessRouter[overlayAccessRouterNum]
            parameters:
                overlayType = overlayType,    // the overlay used in the simulation
                tier1Type = tier1Type,    // the application on top off the overly used in the simulation
                tier2Type = tier2Type,    // tier 2 module to use
                tier3Type = tier3Type;    // tier 3 module to use
            display: "i=device/server2,gray,30;i2=block/circle_s";
        overlayBackboneRouter: OverlayRouter[overlayBackboneRouterNum]
            parameters:
                overlayType = overlayType,    // the overlay used in the simulation
                tier1Type = tier1Type,    // the application on top off the overly used in the simulation
                tier2Type = tier2Type,    // tier 2 module to use
                tier3Type = tier3Type;    // tier 3 module to use
            display: "i=device/server2;i2=block/circle_s";
        globalObserver: GlobalObserver;
            display: "i=block/control";
	channelcontrol: ChannelControl; 
            parameters:
                playgroundSizeX = playgroundSizeX,
                playgroundSizeY = playgroundSizeY;
            display: "p=60,50;i=misc/cloud";
        	    
    connections nocheck:

        //
        // BACKBONE ROUTER
        //
        // connect outRouter to a backboneRouter
            backboneRouter[0].out++ --> fiberline --> outRouter[0].in++ if outRouterNum > 0;
            backboneRouter[0].in++ <-- fiberline <-- outRouter[0].out++ if outRouterNum > 0;

        // connect backboneRouters with other backboneRouters
//         for i=0..backboneRouterNum - 1, j=0..backboneRouterNum - 1 do
//             backboneRouter[i].out++ --> fiberline --> backboneRouter[j].in++ if i < j && uniform2(0, 1, i, 1) <= connectivity;
//             backboneRouter[i].in++ <-- fiberline <-- backboneRouter[j].out++ if i < j && uniform2(0, 1, i, 0) <= connectivity;
//         endfor;
	for i=0..backboneRouterNum - 1, j=0..backboneRouterNum - 1 do
            backboneRouter[i].out++ --> fiberline --> backboneRouter[j].in++ if i < j; //&& uniform2(0, 1, i, 1) <= connectivity;
            backboneRouter[i].in++ <-- fiberline <-- backboneRouter[j].out++ if i < j; //&& uniform2(0, 1, i, 0) <= connectivity;
        endfor;

	//
        // GATEWAY ROUTER
        //
	// connect gatewayRouters with backboneRouters
// 	for i=0..gatewayRouterNum - 1 do
//             gatewayRouter[i].out++ --> fiberline --> backboneRouter[intuniform2(0, backboneRouterNum - 1, 0, 1)].in++ if uniform2(0, 1, i, 1) > overlayBackboneRouterNum / (overlayBackboneRouterNum + backboneRouterNum);
//             gatewayRouter[i].in++ <-- fiberline <-- backboneRouter[intuniform2(0, backboneRouterNum - 1, 0, 0)].out++ if uniform2(0, 1, i, 0) > overlayBackboneRouterNum / (overlayBackboneRouterNum + backboneRouterNum);
//         endfor;

// 	for i=0..gatewayRouterNum - 1 do
//             gatewayRouter[i].out++ --> fiberline --> backboneRouter[i].in++; 
//             gatewayRouter[i].in++ <-- fiberline <-- backboneRouter[i].out++;
//         endfor;

	/// Gateway routers with fiberline connection to BB routers
// 	gatewayRouter[0].out++ --> fiberline --> backboneRouter[0].in++;
//         gatewayRouter[0].in++ <-- fiberline <-- backboneRouter[0].out++;
// 
// 	gatewayRouter[1].out++ --> fiberline --> backboneRouter[0].in++;
//         gatewayRouter[1].in++ <-- fiberline <-- backboneRouter[0].out++;
// 
// 	gatewayRouter[2].out++ --> fiberline --> backboneRouter[0].in++;
//         gatewayRouter[2].in++ <-- fiberline <-- backboneRouter[0].out++;
// 
// 	gatewayRouter[3].out++ --> fiberline --> backboneRouter[1].in++;
//         gatewayRouter[3].in++ <-- fiberline <-- backboneRouter[1].out++;
// 
// 	gatewayRouter[4].out++ --> fiberline --> backboneRouter[2].in++;
//         gatewayRouter[4].in++ <-- fiberline <-- backboneRouter[2].out++;
// 
// 	gatewayRouter[5].out++ --> fiberline --> backboneRouter[1].in++;
//         gatewayRouter[5].in++ <-- fiberline <-- backboneRouter[1].out++;
// 
// 	gatewayRouter[6].out++ --> fiberline --> backboneRouter[2].in++;
//         gatewayRouter[6].in++ <-- fiberline <-- backboneRouter[2].out++;
// 
// 	gatewayRouter[7].out++ --> fiberline --> backboneRouter[2].in++;
//         gatewayRouter[7].in++ <-- fiberline <-- backboneRouter[2].out++;
// 
// 	gatewayRouter[8].out++ --> fiberline --> backboneRouter[1].in++;
//         gatewayRouter[8].in++ <-- fiberline <-- backboneRouter[1].out++;
// 
// 	gatewayRouter[9].out++ --> fiberline --> backboneRouter[2].in++;
//         gatewayRouter[9].in++ <-- fiberline <-- backboneRouter[2].out++;
// 
// 	gatewayRouter[10].out++ --> fiberline --> backboneRouter[1].in++;
//         gatewayRouter[10].in++ <-- fiberline <-- backboneRouter[1].out++;

	/// Gateway routers with dsl connection to BB routers
	gatewayRouter[0].out++ --> dsl --> backboneRouter[0].in++;
        gatewayRouter[0].in++ <-- dsl <-- backboneRouter[0].out++;

	gatewayRouter[1].out++ --> dsl --> backboneRouter[0].in++;
        gatewayRouter[1].in++ <-- dsl <-- backboneRouter[0].out++;

	gatewayRouter[2].out++ --> dsl --> backboneRouter[0].in++;
        gatewayRouter[2].in++ <-- dsl <-- backboneRouter[0].out++;

	gatewayRouter[3].out++ --> dsl --> backboneRouter[1].in++;
        gatewayRouter[3].in++ <-- dsl <-- backboneRouter[1].out++;

	gatewayRouter[4].out++ --> dsl --> backboneRouter[2].in++;
        gatewayRouter[4].in++ <-- dsl <-- backboneRouter[2].out++;

	gatewayRouter[5].out++ --> dsl --> backboneRouter[1].in++;
        gatewayRouter[5].in++ <-- dsl <-- backboneRouter[1].out++;

	gatewayRouter[6].out++ --> dsl --> backboneRouter[2].in++;
        gatewayRouter[6].in++ <-- dsl <-- backboneRouter[2].out++;

	gatewayRouter[7].out++ --> dsl --> backboneRouter[2].in++;
        gatewayRouter[7].in++ <-- dsl <-- backboneRouter[2].out++;

	gatewayRouter[8].out++ --> dsl --> backboneRouter[1].in++;
        gatewayRouter[8].in++ <-- dsl <-- backboneRouter[1].out++;

	gatewayRouter[9].out++ --> dsl --> backboneRouter[2].in++;
        gatewayRouter[9].in++ <-- dsl <-- backboneRouter[2].out++;

	gatewayRouter[10].out++ --> fiberline --> backboneRouter[1].in++;
        gatewayRouter[10].in++ <-- fiberline <-- backboneRouter[1].out++;



        // connect backboneRouters with overlayBackboneRouters
        for i=0..backboneRouterNum - 1, j=0..overlayBackboneRouterNum - 1 do
            backboneRouter[i].out++ --> fiberline --> overlayBackboneRouter[j].in++ if i <= j && uniform2(0, 1, i, 0) <= connectivity;
            backboneRouter[i].in++ <-- fiberline <-- overlayBackboneRouter[j].out++ if i <= j && uniform2(0, 1, i, 0) <= connectivity;
        endfor;

        // connect overlayBackboneRouters with backboneRouters
        for i=0..overlayBackboneRouterNum - 1, j=0..backboneRouterNum - 1 do
            overlayBackboneRouter[i].out++ --> fiberline --> backboneRouter[j].in++ if i < j && uniform2(0, 1, i, 1) <= connectivity;
            overlayBackboneRouter[i].in++ <-- fiberline <-- backboneRouter[j].out++ if i < j && uniform2(0, 1, i, 0) <= connectivity;
        endfor;

        // connect overlayBackboneRouters with other overlayBackboneRouters
        for i=0..overlayBackboneRouterNum - 1, j=0..overlayBackboneRouterNum - 1 do
            overlayBackboneRouter[i].out++ --> fiberline --> overlayBackboneRouter[j].in++ if i < j && uniform2(0, 1, i, 0) <= connectivity;
            overlayBackboneRouter[i].in++ <-- fiberline <-- overlayBackboneRouter[j].out++ if i < j && uniform2(0, 1, i, 0) <= connectivity;
        endfor;

        //
        // ACCESS ROUTER
        //
       // connect accessRouters with backboneRouters
//         for i=0..accessRouterNum - 1 do
//             accessRouter[i].out++ --> fiberline --> backboneRouter[intuniform2(0, backboneRouterNum - 1, 0, 1)].in++ if uniform2(0, 1, i, 1) > overlayBackboneRouterNum / (overlayBackboneRouterNum + backboneRouterNum);
//             accessRouter[i].in++ <-- fiberline <-- backboneRouter[intuniform2(0, backboneRouterNum - 1, 0, 0)].out++ if uniform2(0, 1, i, 0) > overlayBackboneRouterNum / (overlayBackboneRouterNum + backboneRouterNum);
//         endfor;
	// connect accessRouters with gatewayRouters
// 	for i=0..accessRouterNum - 1 do
//             accessRouter[i].out++ --> fiberline --> gatewayRouter[intuniform2(0, gatewayRouterNum - 1, 0, 1)].in++ if uniform2(0, 1, i, 1) > overlayBackboneRouterNum / (overlayBackboneRouterNum + gatewayRouterNum);
//             accessRouter[i].in++ <-- fiberline <-- gatewayRouter[intuniform2(0, gatewayRouterNum - 1, 0, 0)].out++ if uniform2(0, 1, i, 0) > overlayBackboneRouterNum / (overlayBackboneRouterNum + gatewayRouterNum);
//         endfor;

// 	for i=0..accessRouterNum - 1 do
//             accessRouter[i].out++ --> fiberline --> gatewayRouter[i].in++; 
//             accessRouter[i].in++ <-- fiberline <-- gatewayRouter[i].out++;
//         endfor;

	accessRouter[0].out++ --> fiberline --> gatewayRouter[10].in++;
 	accessRouter[0].in++ <-- fiberline <-- gatewayRouter[10].out++;
	
// 	accessRouter[0].out++ --> fiberline --> gatewayRouter[0].in++;
//  	accessRouter[0].in++ <-- fiberline <-- gatewayRouter[0].out++;
//  
// 	accessRouter[1].out++ --> fiberline --> gatewayRouter[1].in++;
// 	accessRouter[1].in++ <-- fiberline <-- gatewayRouter[1].out++;
//  
// 	accessRouter[2].out++ --> fiberline --> gatewayRouter[2].in++;
// 	accessRouter[2].in++ <-- fiberline <-- gatewayRouter[2].out++;
// // 
// 	accessRouter[3].out++ --> fiberline --> gatewayRouter[3].in++;
// 	accessRouter[3].in++ <-- fiberline <-- gatewayRouter[3].out++;
// 
// 	accessRouter[4].out++ --> fiberline --> gatewayRouter[4].in++;
// 	accessRouter[4].in++ <-- fiberline <-- gatewayRouter[4].out++;
// 
// 	accessRouter[5].out++ --> fiberline --> gatewayRouter[5].in++;
// 	accessRouter[5].in++ <-- fiberline <-- gatewayRouter[5].out++;
// 
// 	accessRouter[6].out++ --> fiberline --> gatewayRouter[6].in++;
// 	accessRouter[6].in++ <-- fiberline <-- gatewayRouter[6].out++;
// 
// 	accessRouter[7].out++ --> fiberline --> gatewayRouter[7].in++;
// 	accessRouter[7].in++ <-- fiberline <-- gatewayRouter[7].out++;
// 
// 	accessRouter[8].out++ --> fiberline --> gatewayRouter[8].in++;
// 	accessRouter[8].in++ <-- fiberline <-- gatewayRouter[8].out++;
// 
// 	accessRouter[9].out++ --> fiberline --> gatewayRouter[9].in++;
//  	accessRouter[9].in++ <-- fiberline <-- gatewayRouter[9].out++;
//  
// 	accessRouter[10].out++ --> fiberline --> gatewayRouter[0].in++;
// 	accessRouter[10].in++ <-- fiberline <-- gatewayRouter[0].out++;
//  
// 	accessRouter[11].out++ --> fiberline --> gatewayRouter[1].in++;
// 	accessRouter[11].in++ <-- fiberline <-- gatewayRouter[1].out++;
// 
// 	accessRouter[12].out++ --> fiberline --> gatewayRouter[2].in++;
// 	accessRouter[12].in++ <-- fiberline <-- gatewayRouter[2].out++;
// 
// 	accessRouter[13].out++ --> fiberline --> gatewayRouter[3].in++;
// 	accessRouter[13].in++ <-- fiberline <-- gatewayRouter[3].out++;
// 
// 	accessRouter[14].out++ --> fiberline --> gatewayRouter[4].in++;
// 	accessRouter[14].in++ <-- fiberline <-- gatewayRouter[4].out++;
// 
// 	accessRouter[15].out++ --> fiberline --> gatewayRouter[5].in++;
// 	accessRouter[15].in++ <-- fiberline <-- gatewayRouter[5].out++;
// 
// 	accessRouter[16].out++ --> fiberline --> gatewayRouter[6].in++;
// 	accessRouter[16].in++ <-- fiberline <-- gatewayRouter[6].out++;
// 
// 	accessRouter[17].out++ --> fiberline --> gatewayRouter[7].in++;
// 	accessRouter[17].in++ <-- fiberline <-- gatewayRouter[7].out++;
// 
// 	accessRouter[18].out++ --> fiberline --> gatewayRouter[8].in++;
// 	accessRouter[18].in++ <-- fiberline <-- gatewayRouter[8].out++;
// // 
// 	accessRouter[19].out++ --> fiberline --> gatewayRouter[9].in++;
// 	accessRouter[19].in++ <-- fiberline <-- gatewayRouter[9].out++;
// 
// 	accessRouter[20].out++ --> fiberline --> gatewayRouter[0].in++;
// 	accessRouter[20].in++ <-- fiberline <-- gatewayRouter[0].out++;
// // 
// 	accessRouter[21].out++ --> fiberline --> gatewayRouter[1].in++;
// 	accessRouter[21].in++ <-- fiberline <-- gatewayRouter[1].out++;
// 
// 	accessRouter[22].out++ --> fiberline --> gatewayRouter[2].in++;
// 	accessRouter[22].in++ <-- fiberline <-- gatewayRouter[2].out++;
// 
// 	accessRouter[23].out++ --> fiberline --> gatewayRouter[3].in++;
// 	accessRouter[23].in++ <-- fiberline <-- gatewayRouter[3].out++;
// // 
// 	accessRouter[24].out++ --> fiberline --> gatewayRouter[4].in++;
// 	accessRouter[24].in++ <-- fiberline <-- gatewayRouter[4].out++;
// // 
// 	accessRouter[25].out++ --> fiberline --> gatewayRouter[5].in++;
// 	accessRouter[25].in++ <-- fiberline <-- gatewayRouter[5].out++;
// 
// 	accessRouter[26].out++ --> fiberline --> gatewayRouter[6].in++;
// 	accessRouter[26].in++ <-- fiberline <-- gatewayRouter[6].out++;
// 
// 	accessRouter[27].out++ --> fiberline --> gatewayRouter[7].in++;
// 	accessRouter[27].in++ <-- fiberline <-- gatewayRouter[7].out++;
// 
// 	accessRouter[28].out++ --> fiberline --> gatewayRouter[8].in++;
// 	accessRouter[28].in++ <-- fiberline <-- gatewayRouter[8].out++;
// // 
// 	accessRouter[29].out++ --> fiberline --> gatewayRouter[9].in++;
// 	accessRouter[29].in++ <-- fiberline <-- gatewayRouter[9].out++;



	//connect WirelessAccessRouters with backboneRouters (OM)

// 	for i=0..wirelessAccessRouterNum - 1 do
//             wirelessAccessRouter[i].out++ --> fiberline --> backboneRouter[intuniform2(0, backboneRouterNum - 1, 0, 1)].in++ if uniform2(0, 1, i, 1) > overlayBackboneRouterNum / (overlayBackboneRouterNum + backboneRouterNum);
//             wirelessAccessRouter[i].in++ <-- fiberline <-- backboneRouter[intuniform2(0, backboneRouterNum - 1, 0, 0)].out++ if uniform2(0, 1, i, 0) > overlayBackboneRouterNum / (overlayBackboneRouterNum + backboneRouterNum);
//         endfor;

	//connect WirelessAccessRouters with gatewayRouters (OM)
// 	for i=0..wirelessAccessRouterNum - 1 do
//             wirelessAccessRouter[i].out++ --> fiberline --> gatewayRouter[intuniform2(0, gatewayRouterNum - 1, 0, 1)].in++ if uniform2(0, 1, i, 1) > overlayBackboneRouterNum / (overlayBackboneRouterNum + gatewayRouterNum);
//             wirelessAccessRouter[i].in++ <-- fiberline <-- gatewayRouter[intuniform2(0, gatewayRouterNum - 1, 0, 0)].out++ if uniform2(0, 1, i, 0) > overlayBackboneRouterNum / (overlayBackboneRouterNum + gatewayRouterNum);
//         endfor;

// 	for i=0..wirelessAccessRouterNum - 1 do
//             wirelessAccessRouter[i].out++ --> fiberline --> gatewayRouter[i].in++; 
//             wirelessAccessRouter[i].in++ <-- fiberline <-- gatewayRouter[i].out++;
//         endfor;

 	wirelessAccessRouter[0].out++ --> fiberline --> gatewayRouter[0].in++;
 	wirelessAccessRouter[0].in++ <-- fiberline <-- gatewayRouter[0].out++;
 
	wirelessAccessRouter[1].out++ --> fiberline --> gatewayRouter[1].in++;
	wirelessAccessRouter[1].in++ <-- fiberline <-- gatewayRouter[1].out++;
 
	wirelessAccessRouter[2].out++ --> fiberline --> gatewayRouter[2].in++;
	wirelessAccessRouter[2].in++ <-- fiberline <-- gatewayRouter[2].out++;
// 
	wirelessAccessRouter[3].out++ --> fiberline --> gatewayRouter[3].in++;
	wirelessAccessRouter[3].in++ <-- fiberline <-- gatewayRouter[3].out++;

	wirelessAccessRouter[4].out++ --> fiberline --> gatewayRouter[4].in++;
	wirelessAccessRouter[4].in++ <-- fiberline <-- gatewayRouter[4].out++;

	wirelessAccessRouter[5].out++ --> fiberline --> gatewayRouter[5].in++;
	wirelessAccessRouter[5].in++ <-- fiberline <-- gatewayRouter[5].out++;

	wirelessAccessRouter[6].out++ --> fiberline --> gatewayRouter[6].in++;
	wirelessAccessRouter[6].in++ <-- fiberline <-- gatewayRouter[6].out++;

	wirelessAccessRouter[7].out++ --> fiberline --> gatewayRouter[7].in++;
	wirelessAccessRouter[7].in++ <-- fiberline <-- gatewayRouter[7].out++;

	wirelessAccessRouter[8].out++ --> fiberline --> gatewayRouter[8].in++;
	wirelessAccessRouter[8].in++ <-- fiberline <-- gatewayRouter[8].out++;

	wirelessAccessRouter[9].out++ --> fiberline --> gatewayRouter[9].in++;
 	wirelessAccessRouter[9].in++ <-- fiberline <-- gatewayRouter[9].out++;
 
	wirelessAccessRouter[10].out++ --> fiberline --> gatewayRouter[0].in++;
	wirelessAccessRouter[10].in++ <-- fiberline <-- gatewayRouter[0].out++;
 
	wirelessAccessRouter[11].out++ --> fiberline --> gatewayRouter[1].in++;
	wirelessAccessRouter[11].in++ <-- fiberline <-- gatewayRouter[1].out++;

	wirelessAccessRouter[12].out++ --> fiberline --> gatewayRouter[2].in++;
	wirelessAccessRouter[12].in++ <-- fiberline <-- gatewayRouter[2].out++;

	wirelessAccessRouter[13].out++ --> fiberline --> gatewayRouter[3].in++;
	wirelessAccessRouter[13].in++ <-- fiberline <-- gatewayRouter[3].out++;

	wirelessAccessRouter[14].out++ --> fiberline --> gatewayRouter[4].in++;
	wirelessAccessRouter[14].in++ <-- fiberline <-- gatewayRouter[4].out++;

	wirelessAccessRouter[15].out++ --> fiberline --> gatewayRouter[5].in++;
	wirelessAccessRouter[15].in++ <-- fiberline <-- gatewayRouter[5].out++;

	wirelessAccessRouter[16].out++ --> fiberline --> gatewayRouter[6].in++;
	wirelessAccessRouter[16].in++ <-- fiberline <-- gatewayRouter[6].out++;

	wirelessAccessRouter[17].out++ --> fiberline --> gatewayRouter[7].in++;
	wirelessAccessRouter[17].in++ <-- fiberline <-- gatewayRouter[7].out++;

	wirelessAccessRouter[18].out++ --> fiberline --> gatewayRouter[8].in++;
	wirelessAccessRouter[18].in++ <-- fiberline <-- gatewayRouter[8].out++;
// 
	wirelessAccessRouter[19].out++ --> fiberline --> gatewayRouter[9].in++;
	wirelessAccessRouter[19].in++ <-- fiberline <-- gatewayRouter[9].out++;

	wirelessAccessRouter[20].out++ --> fiberline --> gatewayRouter[0].in++;
	wirelessAccessRouter[20].in++ <-- fiberline <-- gatewayRouter[0].out++;
// 
	wirelessAccessRouter[21].out++ --> fiberline --> gatewayRouter[1].in++;
	wirelessAccessRouter[21].in++ <-- fiberline <-- gatewayRouter[1].out++;

	wirelessAccessRouter[22].out++ --> fiberline --> gatewayRouter[2].in++;
	wirelessAccessRouter[22].in++ <-- fiberline <-- gatewayRouter[2].out++;

	wirelessAccessRouter[23].out++ --> fiberline --> gatewayRouter[3].in++;
	wirelessAccessRouter[23].in++ <-- fiberline <-- gatewayRouter[3].out++;
// 
	wirelessAccessRouter[24].out++ --> fiberline --> gatewayRouter[4].in++;
	wirelessAccessRouter[24].in++ <-- fiberline <-- gatewayRouter[4].out++;
// 
	wirelessAccessRouter[25].out++ --> fiberline --> gatewayRouter[5].in++;
	wirelessAccessRouter[25].in++ <-- fiberline <-- gatewayRouter[5].out++;

	wirelessAccessRouter[26].out++ --> fiberline --> gatewayRouter[6].in++;
	wirelessAccessRouter[26].in++ <-- fiberline <-- gatewayRouter[6].out++;

	wirelessAccessRouter[27].out++ --> fiberline --> gatewayRouter[7].in++;
	wirelessAccessRouter[27].in++ <-- fiberline <-- gatewayRouter[7].out++;

	wirelessAccessRouter[28].out++ --> fiberline --> gatewayRouter[8].in++;
	wirelessAccessRouter[28].in++ <-- fiberline <-- gatewayRouter[8].out++;
// 
	wirelessAccessRouter[29].out++ --> fiberline --> gatewayRouter[9].in++;
	wirelessAccessRouter[29].in++ <-- fiberline <-- gatewayRouter[9].out++;



        // connect accessRouters with overlayBackboneRouters
//         for i=0..accessRouterNum - 1 do
//             accessRouter[i].out++ --> fiberline --> overlayBackboneRouter[intuniform2(0, overlayBackboneRouterNum - 1, 0, 1)].in++ if uniform2(0, 1, i, 0) <= overlayBackboneRouterNum / (overlayBackboneRouterNum + backboneRouterNum);
//             accessRouter[i].in++ <-- fiberline <-- overlayBackboneRouter[intuniform2(0, overlayBackboneRouterNum - 1, 0, 0)].out++ if uniform2(0, 1, i, 0) <= overlayBackboneRouterNum / (overlayBackboneRouterNum + backboneRouterNum);
//         endfor;

        // connect overlayAccessRouters with backboneRouters
        for i=0..overlayAccessRouterNum - 1 do
            overlayAccessRouter[i].out++ --> fiberline --> backboneRouter[intuniform2(0, backboneRouterNum - 1, 0, 1)].in++ if uniform2(0, 1, i, 1) > overlayBackboneRouterNum / (overlayBackboneRouterNum + backboneRouterNum);
            overlayAccessRouter[i].in++ <-- fiberline <-- backboneRouter[intuniform2(0, backboneRouterNum - 1, 0, 0)].out++ if uniform2(0, 1, i, 0) > overlayBackboneRouterNum / (overlayBackboneRouterNum + backboneRouterNum);
        endfor;

        // connect overlayAccessRouters with overlayBackboneRouters
        for i=0..overlayAccessRouterNum - 1 do
            overlayAccessRouter[i].out++ --> fiberline --> overlayBackboneRouter[intuniform2(0, overlayBackboneRouterNum - 1, 0, 1)].in++ if uniform2(0, 1, i, 0) <= overlayBackboneRouterNum / (overlayBackboneRouterNum + backboneRouterNum);
            overlayAccessRouter[i].in++ <-- fiberline <-- overlayBackboneRouter[intuniform2(0, overlayBackboneRouterNum - 1, 0, 0)].out++ if uniform2(0, 1, i, 0) <= overlayBackboneRouterNum / (overlayBackboneRouterNum + backboneRouterNum);
        endfor;

endmodule

//
// The IPv4Underlay network definition
//
network IPv4Network : IPv4Underlay		    
endnetwork
