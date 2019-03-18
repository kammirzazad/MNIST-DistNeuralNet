#ifndef MNIST_TYPEDEF_H
#define MNIST_TYPEDEF_H

#include <map>
#include <set>
#include <vector>
#include <fstream>
#include <iterator>
#include <algorithm>
#include "netInfo.h"
#include "udpBuffer.h"
#include <inet/applications/base/ApplicationBase.h>
#include <inet/transportlayer/contract/udp/UDPSocket.h>

template<class T>
using   arr = std::vector<T>;

using   str2 = std::string;
using   uint = unsigned int;
using   sock = inet::UDPSocket;
using   strMap = std::map<str2,str2>;
using   addrMap = std::map<str2,inet::L3Address>;

#define IN_WIDTH    28
#define TILE_WIDTH  4
#define N1          IN_WIDTH*IN_WIDTH
#define N2          128
#define N3          10

#endif
