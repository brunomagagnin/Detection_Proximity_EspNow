#ifndef COMPAREADDRESS_H
#define COMPAREADDRESS_H
    
#include <Arduino.h>
#include "Config.h"
#include "Peer/Peer.h"


namespace compare
{
    bool compareAddressPeer(Peer *obj, char *mac);

}

#endif