#include "ESP/ESP_Core.h"
/*----------------------------------------------------------------------------
----------------------------------- CONSTRUCTORS------------------------------
----------------------------------------------------------------------------*/
ESP_Core::ESP_Core(){}
 
ESP_Core::ESP_Core(char *macAddress) : Peer(macAddress)
{
    this->peerAlert = false;
    this->peerDanger = false;
    this->acceptClose = false;
    this->isCreateIcon = false;
}

/*----------------------------------------------------------------------------
--------------------------------------GET METHODS-----------------------------
----------------------------------------------------------------------------*/

bool ESP_Core::getIsAlert()
{
    return this->peerAlert;
}

bool ESP_Core::getIsDanger()
{
    return this->peerDanger;
}

bool ESP_Core::getAcceptClose()
{
    return this->acceptClose;
}

bool ESP_Core::getIsCreateIcon()
{
    return this->isCreateIcon;
}

u_int32_t ESP_Core::getAlertTime()
{
    return this->alertTime;
}

u_int32_t ESP_Core::getDangerTime()
{
    return this->dangerTime;
}

/*----------------------------------------------------------------------------
--------------------------------------SET METHODS-----------------------------
----------------------------------------------------------------------------*/

void ESP_Core::setIsAlert(bool alert)
{
    this->peerAlert = alert;
}

void ESP_Core::setIsDanger(bool danger)
{
    this->peerDanger = danger;
}
void ESP_Core::setAcceptClose(bool b)
{
    this->acceptClose = b;
}

void ESP_Core::setIsCreateIcon(bool c)
{
    this->isCreateIcon = c;
}

void ESP_Core::setAlertTime(u_int32_t time)
{
    this->alertTime = time;
}

void ESP_Core::setDangerTime(u_int32_t time)
{
    this->dangerTime = time;
} 