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

#include "FCLayer2.h"
#include <inet/applications/base/ApplicationPacket_m.h>

Define_Module(FCLayer2);


void    FCLayer2::handleMessageWhenUp(cMessage* msg)
{
        if(msg->isSelfMessage())
        {
            ASSERT(msg == selfMsg);

            switch( msg->getKind() )
            {
                case PUSH:  { sendVal(); break; }
                case POP:   { setOutVal(); break; }

                default:
                {
                    std::cout << "unknown message kind " << msg->getKind() << std::endl;
                    exit(1);
                }
            }
        }
        else if( msg->getKind() == inet::UDP_I_DATA)
        {
            auto ctrl = chck_and_cast<inet::UDPDataIndication*>(msg->removeControlInfo());

            int port = (uint)ctrl->getDestPort();
            int id = port-10010;

            if((id<0) || (id>7)) // id should be in [0,7]
            {
                std::cout << "Received data from unexpected port number" << std::endl;
                exit(1);
            }

            if(ctrl->getSrcAddr() != netInfo::getIP(id+1))
            {
                std::cout << "Received data from unexpected IP address" << std::endl;
                exit(1);
            }

            buffers[id]->addToken(PK(msg));
        }
        else if( msg->getKind() == inet::UDP_I_ERROR)
        {
            EV_WARN << "Ignoring UDP error report" << std::endl;
            delete msg;
        }
        else
        {
            throw cRuntimeError("Unrecognized message (%s)%s", msg->getClassName(), msg->getName());
        }
}


void    FCLayer2::initialize(int stage)
{
        ApplicationBase::initialize(stage);

        if(stage == inet::INITSTAGE_LOCAL)
        {
            ts = par("startTime");

            for(uint i=0; i<8; i++)
            {
                auto sPtr = new Sock();
                sPtr->setOutputGate(gate("udpOut"));
                sPtr->bind(netInfo::getIP(9),10010+id);

                inSockets.push_back(sPtr);
                buffers.push_back(new udpBuffer(par("mem")));
            }

            selfMsg = new cMessage("scheduler");
        }
}

bool    FCLayer2::handleNodeStart(inet::IDoneCallback *doneCallBack)
{
        std::cout << "FCLayer2 started." << std::endl;

        selfMsg->setKind(POP);
        scheduleAt(ts,selfMsg);
        return true;
}

FCLayer2::~FCLayer2()
{
        if(selfMsg) { cancelEvent(selfMsg); }

        for(sock* socket:inSockets) delete socket;
        for(udpBuffer* buff:buffers) delete buff;

        delete  selfMsg;
}

FCLayer2::FCLayer2()
:   mem(1), ts(0.0)
{}

void    FCLayer2::handleNodeCrash()
{
        std::cout << "FCLayer2 crashed!" << std::endl;
}

bool    FCLayer2::handleNodeShutdown(inet::IDoneCallback *doneCallBack)
{
        std::cout << "FCLayer2 shutdown." << std::endl;
        return true;
}

double  FCLayer2::sigmoid(double x)
{
        return 1.0 / (1.0 + exp(-x));
}

