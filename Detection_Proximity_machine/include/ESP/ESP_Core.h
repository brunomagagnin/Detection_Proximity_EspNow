#pragma once
#include <M5Core2.h>
#include "Peer/Peer.h"
#include "ESP/CoreState.h"
#include <Arduino.h>
#include "Config.h"

class ESP_Core : public Peer, public CoreState
{

private:
    bool isCreateIcon;
    bool controlAlert;

public:
    ESP_Core();
    ESP_Core(char *macAddress);

    bool getIsCreateIcon();

    void setIsCreateIcon(bool c);
};