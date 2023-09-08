#pragma once
#include <Arduino.h> 

class ESP_Peer{

private:
    bool peerAlert;
    bool peerDanger;

    int rssi;
    int index;
    char macAddress[18];
    u_int32_t alertTime;
    u_int32_t dangerTime;
    

public:
    ESP_Peer();
    ESP_Peer(char *macAddress);

    char getMacAddress(int index);
    char* getMacAddress();

    bool getIsAlert();
    bool getIsDanger();

    int getRssi();
    u_int32_t getAlertTime();
    u_int32_t getDangerTime();

    void setIsAlert(bool alert);
    void setIsDanger(bool danger);
    void setRssi(int rssi);
    void setAlertTime(u_int32_t time);
    void setDangerTime(u_int32_t time);
};
