#include "ESP_Peer.h"

/*----------------------------------------------------------------------------
----------------------------------- CONSTRUCTORS------------------------------
----------------------------------------------------------------------------*/

ESP_Peer::ESP_Peer()
{
}

ESP_Peer::ESP_Peer(char *macAddress)
{
    this->rssi = 0;
    this->index = 0;
    this->peerAlert = false;
    this->peerDanger = false;

    for (int i = 0; i < sizeof(this->macAddress); i++)
    {
        this->macAddress[i] = macAddress[i];
    }
}

/*----------------------------------------------------------------------------
--------------------------------------GET METHODS-----------------------------
----------------------------------------------------------------------------*/

char ESP_Peer::getMacAddress(int index)
{
    return this->macAddress[index];
}

char *ESP_Peer::getMacAddress()
{
    return this->macAddress;
}

int ESP_Peer::getRssi()
{
    return rssi;
}

bool ESP_Peer::getIsAlert()
{
    return this->peerAlert;
}

bool ESP_Peer::getIsDanger()
{
    return this->peerDanger;
} 

u_int32_t ESP_Peer::getAlertTime()
{
    return this->alertTime;
}

u_int32_t ESP_Peer::getDangerTime()
{
    return this->dangerTime;
}

/*----------------------------------------------------------------------------------
------------------------------------- SET METHODS ----------------------------------
------------------------------------------------------------------------------------*/

void ESP_Peer::setRssi(int rssi)
{
    this->rssi = rssi;
}

void ESP_Peer::setIsAlert(bool alert)
{
    this->peerAlert = alert;
}

void ESP_Peer::setIsDanger(bool danger)
{
    this->peerDanger = danger;
}

void ESP_Peer::setAlertTime(u_int32_t time)
{
    this->alertTime = time;
}

void ESP_Peer::setDangerTime(u_int32_t time)
{
    this->dangerTime = time;
}
