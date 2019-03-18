#ifndef MNIST_NET_INFO_H
#define MNIST_NET_INFO_H

#include <cstdlib>
#include <inet/networklayer/common/L3Address.h>
//#include <inet/networklayer/common/L3AddressResolver.h>

class   netInfo
{
        public:

        netInfo(uint _hostIdx, uint _port)
        : port(_port), hostIdx(_hostIdx), addr(getIP(_hostIdx))
        {
                        /* nothing to do */
                        //std::cout << "created netInfo with port=" << port << " and host=" << host << std::endl;
        }

        static  inet::L3Address getIP(uint idx)
        {
                        std::string baseIP = "169.254.";
                        idx++; // cannot use "169.254.0.0"
                        baseIP += std::to_string(idx) + '.' + std::to_string(idx); // 169.254.idx.idx
                        return inet::L3Address(baseIP.c_str());
        }

        uint            port;
        uint            hostIdx;
        inet::L3Address addr;
};

#endif
