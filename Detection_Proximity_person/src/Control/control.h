#ifndef CONTROL_H
#define CONTROL_H
#include "Config.h"
#include <Arduino.h>
#include <M5Core2.h>
#include "control.h"
#include "ESP_PEER/ESP_Peer.h"


namespace controller
{
    void alert(ESP_Peer *peers);
}

#endif