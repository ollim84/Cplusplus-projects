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

//
// Configurator of the IPv4Underlay
//
// Sets up an IPv4Network (backbone and access networks), adds overlay nodes to
// the network in init phase periodically with the interval set by
// "initalMobilityDelay", adds/removes/migrates overlay nodes after init
// phase periodically with the interval set by targetMobilityDelay with
// the probabilities defined by the parameters creationProbability,
// removalProbability, and migrationProbability
//
// @author Markus Mauch
//
simple IPv4UnderlayConfigurator
    parameters:
        startIP : string,    // IP configuration starts with this IP
        gatewayIP : string,    // IP of the gateway (if an outRouter is used)
        channelTypes : string,    // used access technologies
        gracefulLeaveDelay : numeric,    // time between prekill and removal from overlay
        gracefulLeaveProbability : numeric,    // probability of graceful leave
        churnGeneratorTypes : string,    // a list of churn generators to be used
        terminalTypes: string,    // list of terminal types
	subtrackerNum : numeric const,
	seederNum : numeric const,
        transitionTime : numeric,    // time to wait before measuring after init phase is finished
        measurementTime : numeric;    // duration of the simulation after init and transition phase
endsimple

