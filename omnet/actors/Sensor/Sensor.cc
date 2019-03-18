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

#include "Sensor.h"

#include <inet/applications/base/ApplicationPacket_m.h>

Define_Module(Sensor);

void    Sensor::sendVal()
{

}

void    Sensor::setOutVal()
{
        char number;
        for (int j = 1; j <= IN_WIDTH; ++j)
        {
            for (int i = 1; i <= IN_WIDTH; ++i)
            {
                image.read(&number, sizeof(char));

                if (number == 0)
                {
                    d[i][j] = 0;
                }
                else
                {
                    d[i][j] = 1;
                }
            }
        }

        for (int j = 1; j <= IN_WIDTH; ++j)
        {
            for (int i = 1; i <= IN_WIDTH; ++i)
            {
                int pos = i + (j - 1) * IN_WIDTH;
                out1[pos] = d[i][j];
            }
        }
}

void    Sensor::initialize(int stage)
{
        ApplicationBase::initialize(stage);

        if(stage == inet::INITSTAGE_LOCAL)
        {
            ts = par("startTime");
            str2 path2image = par("path_to_image");
            image.open(path2image.c_str(), ios::in | ios::binary); // Binary image file

            // Reading file headers
            char number;
            for (int i = 1; i <= 16; ++i)
                image.read(&number, sizeof(char));

            for(uint i=0; i<8; i++)
            {
                auto sPtr = new Sock();
                sPtr->setOutputGate(gate("udpOut"));
                sPtr->connect(netInfo::getIP(id+1),10000+id);

                outSockets.push_back(sPtr);
            }

            selfMsg = new cMessage("scheduler");
        }
}

bool    Sensor::handleNodeStart(inet::IDoneCallback *doneCallBack)
{
        std::cout << "Sensor started." << std::endl;

        selfMsg->setKind(POP);
        scheduleAt(ts,selfMsg);
        return true;
}

Sensor::~Sensor()
{
        if(selfMsg) { cancelEvent(selfMsg); }

        delete  selfMsg;

        image.close();
}

Sensor::Sensor()
:   ts(0.0)
{
        out1 = new double [N1 + 1];
}

void    Sensor::handleNodeCrash()
{
        std::cout << "Sensor crashed!" << std::endl;
}

bool    Sensor::handleNodeShutdown(inet::IDoneCallback *doneCallBack)
{
        std::cout << "Sensor shutdown." << std::endl;
        return true;
}

