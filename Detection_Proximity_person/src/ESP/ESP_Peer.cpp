#include "ESP/ESP_Peer.h"
/*----------------------------------------------------------------------------
----------------------------------- CONSTRUCTORS------------------------------
----------------------------------------------------------------------------*/
ESP_Peer::ESP_Peer(){}
 
ESP_Peer::ESP_Peer(char *macAddress) : Peer(macAddress)
{
    this->peerAlert = false;
    this->peerDanger = false;
    this->isCreateIcon = false;
}

/*----------------------------------------------------------------------------
--------------------------------------GET METHODS-----------------------------
----------------------------------------------------------------------------*/

bool ESP_Peer::getIsAlert()
{
    return this->peerAlert;
}

bool ESP_Peer::getIsDanger()
{
    return this->peerDanger;
}

bool ESP_Peer::getIsCreateIcon()
{
    return this->isCreateIcon;
}

u_int32_t ESP_Peer::getAlertTime()
{
    return this->alertTime;
}

u_int32_t ESP_Peer::getDangerTime()
{
    return this->dangerTime;
}

/*----------------------------------------------------------------------------
--------------------------------------SET METHODS-----------------------------
----------------------------------------------------------------------------*/

void ESP_Peer::setIsAlert(bool alert)
{
    this->peerAlert = alert;
}

void ESP_Peer::setIsDanger(bool danger)
{
    this->peerDanger = danger;
}


void ESP_Peer::setIsCreateIcon(bool c)
{
    this->isCreateIcon = c;
}

void ESP_Peer::setAlertTime(u_int32_t time)
{
    this->alertTime = time;
}

void ESP_Peer::setDangerTime(u_int32_t time)
{
    this->dangerTime = time;
} 
