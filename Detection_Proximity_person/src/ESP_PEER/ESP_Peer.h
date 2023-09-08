#pragma once
#include <Arduino.h>

class ESP_Peer{

private:
    bool peerAlert;
    bool peerDanger;
    bool acceptClose;
    bool isCreateIcon;

    int rssi;
    int index;
    char macAddress[18];
    u_int32_t alertTime;
    u_int32_t dangerTime;
    int rssiArray[4];

public:
    ESP_Peer();
    ESP_Peer(char *macAddress);

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
