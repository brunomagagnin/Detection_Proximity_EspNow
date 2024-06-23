#pragma once
#include "Peer/Peer.h"
#include "ESP/CoreState.h"
#include <Arduino.h>
#include "Config.h"

class ESP_Core : public Peer, public CoreState
{

private:
    bool isCreateIcon;
    bool alert;

public:
    ESP_Core();
    ESP_Core(char *macAddress);

    bool getIsCreateIcon();
    bool getIsAlert();

    void setIsCreateIcon(bool c);
    void setIsAlert(bool a);
};