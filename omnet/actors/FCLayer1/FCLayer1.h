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

#ifndef MNIST_FCLAYER1_H
#define MNIST_FCLAYER1_H

#include "../include/typedefs.h"

#define IN_WIDTH    28
#define TILE_WIDTH  4
#define N1          IN_WIDTH*IN_WIDTH

class INET_API FCLayer1 : public inet::ApplicationBase
{
    private:

    enum            SelfMsgKinds { POP=1, PUSH };

    uint            id, iterCnt;
    sock*           inSock, outSock;
    nnData          partialIn2;
    double          ts;
    double          out1[N1];
    netInfo*        inInfo, outInfo;
    cMessage*       selfMsg;
    udpBuffer*      buffer;

    void            sendVal();
    void            setOutVal();
    double          sigmoid(double x);

    protected:

    virtual void    initialize(int stage) override;
    virtual int     numInitStages() const override { return inet::NUM_INIT_STAGES; }
    virtual void    handleNodeCrash() override;
    virtual void    handleMessageWhenUp(cMessage *msg) override;
    virtual bool    handleNodeStart(inet::IDoneCallback *doneCallback) override;
    virtual bool    handleNodeShutdown(inet::IDoneCallback *doneCallback) override;

    public:

    FCLayer1();
    virtual ~FCLayer1();
};

#endif /* ACTORS_FCLAYER1_FCLAYER1_H_ */
