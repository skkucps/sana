//
// Copyright (C) 2012 David Eckhoff <eckhoff@cs.fau.de>
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

#ifndef ___TMAC_H_
#define ___TMAC_H_

#include <assert.h>
#include <omnetpp.h>
#include <queue>
#include <BaseLayer.h>
#include <MacToPhyControlInfo.h>
#include <PhyLayer80211p.h>
#include <WaveAppToMac1609_4Interface.h>
#include <Consts80211p.h>
#include "FindModule.h"
#include <Mac80211Pkt_m.h>
#include <TMACPkt_m.h>
#include <WaveShortMessage_m.h>
#include <TAF_m.h>

#include <BaseMacLayer.h>

/**
 * @brief
 * Manages timeslots for CCH and SCH listening and sending.
 *
 * @author David Eckhoff : rewrote complete model
 * @author Christoph Sommer : features and bug fixes
 * @author Michele Segata : features and bug fixes
 * @author Stefan Joerer : features and bug fixes
 * @author Christopher Saloman: initial version
 *
 * @ingroup macLayer
 *
 * @see BaseWaveApplLayer
 * @see Mac1609_4
 * @see PhyLayer80211p
 * @see Decider80211p
 */
class TMAC : public BaseMacLayer,	public WaveAppToMac1609_4Interface {
public:

    enum t_access_category {
        AC_BK = 0,
        AC_BE = 1,
        AC_VI = 2,
        AC_VO = 3
    };

    class EDCA {
        public:
            class EDCAQueue {
                public:

                    std::queue<WaveShortMessage*> queue;
                    int aifsn; //number of aifs slots for this queue
                    int cwMin; //minimum contention window
                    int cwMax; //maximum contention size
                    int cwCur; //current contention window
                    int currentBackoff; //current Backoff value for this queue
                    bool txOP;

                    EDCAQueue() {	};
                    EDCAQueue(int aifsn,int cwMin, int cwMax, t_access_category ac):aifsn(aifsn),cwMin(cwMin),cwMax(cwMax),cwCur(cwMin),currentBackoff(0),txOP(false) {
                    };
            };

            EDCA(t_channel channelType,int maxQueueLength = 0):numQueues(0),maxQueueSize(maxQueueLength),channelType(channelType) {
                statsNumInternalContention = 0;
                statsNumBackoff = 0;
                statsSlotsBackoff = 0;
            };
            /*
             * Currently you have to call createQueue in the right order. First Call is priority 0, second 1 and so on...
             */
            int createQueue(int aifsn, int cwMin, int cwMax,t_access_category);
            int queuePacket(t_access_category AC,WaveShortMessage* cmsg);
            void backoff(t_access_category ac);
            simtime_t startContent(simtime_t idleSince, bool guardActive);
            void stopContent(bool allowBackoff, bool generateTxOp);
            void postTransmit(t_access_category);
            void revokeTxOPs();

            void cleanUp();

            /** @brief return the next packet to send, send all lower Queues into backoff */
            WaveShortMessage* initiateTransmit(simtime_t idleSince);

        public:
            std::map<t_access_category,EDCAQueue> myQueues;
            int numQueues;
            uint32_t maxQueueSize;
            simtime_t lastStart; //when we started the last contention;
            t_channel channelType;

            /** @brief Stats */
            long statsNumInternalContention;
            long statsNumBackoff;
            long statsSlotsBackoff;

            /** @brief Id for debug messages */
            std::string myId;
    };

public:
    ~TMAC() { };

    void changeServiceChannel(int channelNumber);


protected:
    /** @brief States of the channel selecting operation.*/

protected:
    /** @brief Initialization of the module and some variables.*/
    virtual void initialize(int);

    /** @brief Delete all dynamically allocated objects of the module.*/
    virtual void finish();

    /** @brief Handle messages from lower layer.*/
    virtual void handleLowerMsg(cMessage*);

    /** @brief Handle messages from upper layer.*/
    virtual void handleUpperMsg(cMessage*);

    /** @brief Handle control messages from upper layer.*/
    virtual void handleUpperControl(cMessage* msg);


    /** @brief Handle self messages such as timers.*/
    virtual void handleSelfMsg(cMessage*);

    /** @brief Handle control messages from lower layer.*/
    virtual void handleLowerControl(cMessage* msg);

    /** @brief Set a state for the channel selecting operation.*/
    void setActiveChannel(t_channel state);

    simtime_t timeLeftInSlot() const;
    simtime_t timeLeftTillGuardOver() const;

    bool guardActive() const;

    void attachSignal(Mac80211Pkt* mac, simtime_t startTime, double frequency);
    Signal* createSignal(simtime_t start, simtime_t length, double power, double bitrate, double frequency);

    /** @brief maps a application layer priority (up) to an EDCA access category. */
    t_access_category mapPriority(int prio);

    void channelBusy();
    void channelBusySelf(bool generateTxOp);
    void channelIdle(bool afterSwitch = false);

    void checkBitrate(int bitrate)  const;

protected:
    /** @brief Self message to indicate that the current channel shall be switched.*/
    cMessage* nextChannelSwitch;

    /** @brief Self message to wake up at next MacEvent */
    cMessage* nextMacEvent;

    /** @brief Last time the channel went idle */
    simtime_t lastIdle;
    simtime_t lastBusy;

    /** @brief Current state of the channel selecting operation.*/
    t_channel activeChannel;

    /** @brief access category of last sent packet */
    t_access_category lastAC;

    /** @brief Stores the frequencies in Hz that are associated to the channel numbers.*/
    std::map<int,double> frequency;

    int headerLength;

    bool useSCH;
    int mySCH;

    std::map<t_channel,EDCA*> myEDCA;

    bool idleChannel;

    /** @brief stats */
    long statsReceivedPackets;
    long statsReceivedBroadcasts;
    long statsSentPackets;
    long statsTXRXLostPackets;
    long statsSNIRLostPackets;
    long statsDroppedPackets;
    long statsNumTooLittleTime;
    long statsNumInternalContention;
    long statsNumBackoff;
    long statsSlotsBackoff;
    simtime_t statsTotalBusyTime;

    /** @brief This MAC layers MAC address.*/
    int myMacAddress;

    /** @brief The power (in mW) to transmit with.*/
    double txPower;

    /** @brief the bit rate at which we transmit */
    double bitrate;

    /** @brief Id for debug messages */
    std::string myId;

    Mac80211pToPhy11pInterface* phy11p;

    /*****************
     * 2014_03_03
     * Sangsoo Jeong : Additional functions & variables will be placed below
     *
     */
public:
    class cMsg {
            private:
                int m_iType;
                cMessage* m_pMsg;
            public:
                cMsg(int type = 0,cMessage* msg = NULL)
                {
                    m_iType = type;
                    m_pMsg  = msg;
                }
                int GetType() { return m_iType; }
                cMessage* GetMessage() { return m_pMsg; }
            };
private:
    enum ACCESS_MODE {AM_RSU = 0 , AM_VEHICLE_CFP , AM_VEHICLE_CP};
    enum MSG_TYPE {MT_WSM = 0 , MT_TAF };

    int m_iAccessMode;          //
    int m_iParticipants[20];
    int m_iMACAddrArraySize;

    std::queue<cMsg*> m_oMessageQueue;

    cMessage* nextTAFEvent;
public:
    WaveShortMessage*  prepareWSM(std::string name, int lengthBits, t_channel channel, int priority, int rcvId, int serial);
    TAF* prepareTAF();
    void SendInCP();
    void SendInCFP(cMsg*);

    void OnTAF(TAF* taf);
};

#endif /* ___TMAC_H_*/
