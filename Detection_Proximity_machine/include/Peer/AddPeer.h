#ifndef ADDPEER_H
#define ADDPEER_H

#include <Arduino.h>
#include <esp_wifi.h>
#include <esp_now.h>

#include "Config.h"
#include "ESP/ESP_Core.h"


namespace AddPeer{

void addPersonSlave(uint8_t macSlaves[NUMBER_OF_PERSON][6]);
void addBroadcast();

}

#endif