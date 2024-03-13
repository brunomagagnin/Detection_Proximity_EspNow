#ifndef CONTROL_H
#define CONTROL_H

#include "Config.h"
#include <Arduino.h>
#include <M5Core2.h>
#include "control.h"
#include "ESP/ESP_Peer.h"
#include "ESP/ESP_Core.h"

namespace controller
{
    void alert(ESP_Core *core[NUMBER_OF_MACHINES], bool accepted);
    void peerAlert(ESP_Peer *peer[NUMBER_OF_MACHINES][NUMBER_OF_EXTERN_PEERS], ESP_Core *core[NUMBER_OF_MACHINES]);

    bool counterTime(ESP_Peer *peers);
    bool iconAlert(ESP_Core *core, bool accepted);
}

#endif