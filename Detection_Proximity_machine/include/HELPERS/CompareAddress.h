#ifndef COMPAREADDRESS_H
#define COMPAREADDRESS_H
        
#include <Arduino.h>
#include "Config.h"
#include "ESP/ESP_Core.h"


namespace compare {

    bool addressPeer(ESP_Core *obj, char *mac);

}

#endif