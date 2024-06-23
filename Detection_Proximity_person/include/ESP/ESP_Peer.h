#pragma once
#include "Peer/Peer.h"
#include <Arduino.h>
#include "Config.h"

class ESP_Peer: public Peer{

private:
    bool peerAlert;
    bool peerDanger;
    bool isCreateIcon;

    u_int32_t alertTime;
    u_int32_t dangerTime;


public:
    ESP_Peer();
    ESP_Peer(char *macAddress);

    bool getIsAlert();
    u_int32_t getAlertTime();

    void setIsAlert(bool alert);
    void setAlertTime(u_int32_t time);
};