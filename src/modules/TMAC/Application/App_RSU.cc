//
// Copyright (C) 2006-2011 Christoph Sommer <christoph.sommer@uibk.ac.at>
//
// Documentation for these modules is at http://veins.car2x.org/
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#include "TMAC/Application/App_RSU.h"

Define_Module(App_RSU);

void App_RSU::initialize(int stage) {
	BaseWaveApplLayer::initialize(stage);
	if (stage == 0) {
		sentMessage = false;
		lastDroveAt = simTime();

		nextEvent   =   new cMessage("msg");
		scheduleAt(simTime()+0.1,nextEvent);
	}
}

void App_RSU::onBeacon(WaveShortMessage* wsm) {
}

void App_RSU::onData(WaveShortMessage* wsm) {
	findHost()->getDisplayString().updateWith("r=16,green");
	if (!sentMessage) sendMessage();
}

void App_RSU::sendMessage() {
	sentMessage = true;

	t_channel channel = dataOnSch ? type_SCH : type_CCH;

	sendWSM(prepareWSM("data", dataLengthBits, channel, dataPriority, -1,2));
}


void App_RSU::handlePositionUpdate(cObject* obj) {
	BaseWaveApplLayer::handlePositionUpdate(obj);
	// stopped for for at least 10s?
			if (simTime() - lastDroveAt >= 10) {
			findHost()->getDisplayString().updateWith("r=16,red");
			if (!sentMessage) sendMessage();
		}
}

void App_RSU::handleSelfMsg(cMessage* msg)
{
 //   sendMessage();
    scheduleAt(simTime()+0.1,nextEvent);

}
