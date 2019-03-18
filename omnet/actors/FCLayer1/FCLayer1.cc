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

#include "FCLayer1.h"

#include <cmath>
#include <cctype>
#include <fstream>
#include "nnPacket_m.h"

Define_Module(FCLayer1);

void    FCLayer1::setOutVal()
{
        buffer->waitForToken(IN_WIDTH*IN_WIDTH);

        for(int i=0; i<N1; i++)
        {
            uint row = i/IN_WIDTH;          // [0,27]
            uint col = i%IN_WIDTH;          // [0,27]
            uint tileX = col/TILE_WIDTH;    // [0,7]
            uint tileY = row/TILE_WIDTH;    // [0,7]
            uint offsetX = col%TILE_WIDTH;  // [0,3]
            uint offsetY = row%TILE_WIDTH;  // [0,3]
            uint tileIdx = (IN_WIDTH*tileY)+tileX;  // [0,48]: send tiles in row first manner
            uint offset = (TILE_WIDTH*offsetY) + offsetX;

            token& tile = buffer->readToken(tileIdx);

            if( tile.isEmpty() )
            {
                out1[i] = 0.0;
            }
            else
            {
                out1[i] = tile.getData().array[offset];
            }
        }

        for(int i=0; i<16; i++)
        {
            partialIn2.array[i] = 0.0;

            for(int j=0; j<N1; j++)
                partialIn2.array[i] += out1[j] * w1[j+1][(16*id)+i+1];

            partialIn2.array[i] = sigmoid(partialIn2.array[i]);
        }

        buffer->popToken(IN_WIDTH*IN_WIDTH);

        selfMsg->setKind(PUSH);
        scheduleAt(simTime()+0.005, selfMsg);
}

void    FCLayer1::sendVal()
{
        nnPacket *msg = new nnPacket("token");
        msg->setByteLength(sizeof(partialIn2)+sizeof(uint));
        msg->setSequenceNumber(iterCnt);
        msg->setPayload(partialIn2);
        outSock->send(msg);

        iterCnt++;
        if(iterCnt < 10000)
        {
            selfMsg->setKind(POP);
            scheduleAt(simTime()+0.005, selfMsg);
        }
}

void    FCLayer1::handleMessageWhenUp(cMessage* msg)
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
            auto ctrl = check_and_cast<inet::UDPDataIndication*>(msg->removeControlInfo());
            if(ctrl->getSrcAddr() != netInfo::getIP(0)) // h0 hosts input
            {
                std::cout << "Received data from unexpected IP address" << std::endl;
                exit(1);
            }
            if((uint)ctrl->getDestPort() != (10000+id))
            {
                std::cout << "Received data from unexpected port number" << std::endl;
                exit(1);
            }

            buffer->addToken(PK(msg));
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

void    FCLayer1::initialize(int stage)
{
        ApplicationBase::initialize(stage);

        if(stage == inet::INITSTAGE_LOCAL)
        {
            id = par("id");
            ts = par("startTime");
            inSock = new sock();
            outSock = new sock();
            int mem = par("mem");
            buffer = new udpBuffer(49*mem);                 // 28*28 = 16*49
            selfMsg = new cMessage("scheduler");
            inSock->setOutputGate(gate("udpOut"));
            outSock->setOutputGate(gate("udpOut"));
            inSock->bind(netInfo::getIP(id+1),10000+id);    // id:0=>(h1,10000), id:1=>(h2,10001), ...
            outSock->connect(netInfo::getIP(9),10010+id);   // h9 hosts FC2

            loadWeights(par("path_to_model"));
        }
}

bool    FCLayer1::handleNodeStart(inet::IDoneCallback *doneCallBack)
{
        std::cout << "FCLayer1 (id=" << id << ") started." << std::endl;

        selfMsg->setKind(POP);
        scheduleAt(ts,selfMsg);
        return true;
}

FCLayer1::~FCLayer1()
{
        if(selfMsg) { cancelEvent(selfMsg); }

        delete  inSock;
        delete  buffer;
        delete  outSock;
        delete  selfMsg;
}

FCLayer1::FCLayer1()
:   id(0), iterCnt(0), ts(0.0)
{
        for (int i = 1; i <= N1; ++i)
        {
            w1[i] = new double [N2 + 1];
        }

}

void    FCLayer1::handleNodeCrash()
{
        std::cout << "FCLayer1 (id=" << id << ") crashed!" << std::endl;
}

bool    FCLayer1::handleNodeShutdown(inet::IDoneCallback *doneCallBack)
{
        std::cout << "FCLayer1 (id=" << id << ") shutdown." << std::endl;
        return true;
}

double  FCLayer1::sigmoid(double x)
{
        return 1.0 / (1.0 + exp(-x));
}

void    FCLayer1::loadWeights(str2 file_name)
{
        double temp;
        std::ifstream file(file_name.c_str(), ios::in);

        // Input layer - Hidden layer
        for (int i = 1; i <= N1; ++i)
        {
            for (int j = 1; j <= N2; ++j)
            {
                file >> w1[i][j];
            }
        }

        // Hidden layer - Output layer
        for (int i = 1; i <= N2; ++i)
        {
            for (int j = 1; j <= N3; ++j)
            {
                file >> temp; //w2[i][j];
            }
        }

        file.close();
}
