#ifndef COMPAREADDRESS_H
#define COMPAREADDRESS_H

#include <Arduino.h>
#include "Config.h"
#include "Peer/Peer.h"
#include "ESP/ESP_Peer.h"

namespace compare
{
    bool compareAddressPeer(Peer *obj, char *mac);

    bool compareAddressESP(ESP_Peer *obj, char *mac);

}

#endif