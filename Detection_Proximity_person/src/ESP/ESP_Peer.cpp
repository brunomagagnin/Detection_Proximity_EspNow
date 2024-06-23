#include "ESP/ESP_Peer.h"
/*************************************************************************
------------------------------- CONSTRUCTORS------------------------------
**************************************************************************/
ESP_Peer::ESP_Peer(){}
 
ESP_Peer::ESP_Peer(char *macAddress) : Peer(macAddress)
{
    this->peerAlert = false;

}

/************************************************************************
---------------------------------GET METHODS-----------------------------
*************************************************************************/

bool ESP_Peer::getIsAlert()
{
    return this->peerAlert;
}

u_int32_t ESP_Peer::getAlertTime()
{
    return this->alertTime;
}

/**************************************************************************
-----------------------------------SET METHODS-----------------------------
***************************************************************************/

void ESP_Peer::setIsAlert(bool alert)
{
    this->peerAlert = alert;
}

void ESP_Peer::setAlertTime(u_int32_t time)
{
    this->alertTime = time;
}

