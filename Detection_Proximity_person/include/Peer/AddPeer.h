#ifndef ADDPEER_H
#define ADDPEER_H

#include <Arduino.h>
#include <esp_wifi.h>
#include <esp_now.h>

#include "Config.h"
#include "Peer/Peer.h"
#include "ESP/ESP_Core.h"
#include "ESP/ESP_Peer.h"


namespace AddPeer{

void addPeerSlave(uint8_t macSlaves[NUMBER_OF_MACHINES][NUMBER_OF_EXTERN_PEERS][6]);
void addMachineSlave(uint8_t macSlaves[NUMBER_OF_MACHINES][6]);

}

#endif