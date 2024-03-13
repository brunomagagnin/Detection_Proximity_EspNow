#pragma once
#include "Peer/Peer.h"
#include <Arduino.h>
#include "Config.h"

class ESP_Core: public Peer{

private:
    bool peerAlert;
    bool peerDanger;
    bool acceptClose;
    bool isCreateIcon;

    u_int32_t alertTime;
    u_int32_t dangerTime;


public:
    ESP_Core();
    ESP_Core(char *macAddress);

    char getMacAddress(int index);
    char* getMacAddress();

    bool getIsAlert();
    bool getIsDanger();
    bool getAcceptClose();
    bool getIsCreateIcon();

    int getRssi();
    int getRssiArray(int index);
    u_int32_t getAlertTime();
    u_int32_t getDangerTime();

    void setAcceptClose(bool b);
    void setIsCreateIcon(bool c);

    void setIsAlert(bool alert);
    void setIsDanger(bool danger);

    void setRssi(int rssi);

    void setAlertTime(u_int32_t time);
    void setDangerTime(u_int32_t time);
};