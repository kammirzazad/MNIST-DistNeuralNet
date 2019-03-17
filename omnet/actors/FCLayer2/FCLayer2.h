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

#ifndef MNIST_FCLAYER2_H
#define MNIST_FCLAYER2_H

#include "../include/typedefs.h"

class INET_API FCLayer2 : public inet::ApplicationBase
{
    private:

    enum            SelfMsgKinds { POP=1, PUSH };

    int             mem;
    str2            path_to_label;
    double          ts;
    arr<sock*>      inSockets;
    arr<udpBuffer*> buffers;


    protected:

    virtual void    initialize(int stage) override;
    virtual int     numInitStages() const override { return inet::NUM_INIT_STAGES; }
    virtual void    handleNodeCrash() override;
    virtual void    handleMessageWhenUp(cMessage *msg) override;
    virtual bool    handleNodeStart(inet::IDoneCallback *doneCallback) override;
    virtual bool    handleNodeShutdown(inet::IDoneCallback *doneCallback) override;

    public:

    FCLayer2();
    virtual ~FCLayer2();
};

#endif /* ACTORS_FCLAYER2_FCLAYER2_H_ */
