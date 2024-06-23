#include "ESP/ESP_Core.h"
/*************************************************************************
------------------------------- CONSTRUCTORS------------------------------
**************************************************************************/
ESP_Core::ESP_Core() {}

ESP_Core::ESP_Core(char *macAddress) : Peer(macAddress)
{
    this->isCreateIcon = false;
    this->alert = false;
}

/************************************************************************
---------------------------------GET METHODS-----------------------------
*************************************************************************/

bool ESP_Core::getIsCreateIcon()
{
    return this->isCreateIcon;
}

bool ESP_Core::getIsAlert()
{
    return this->alert;
}

/**************************************************************************
-----------------------------------SET METHODS-----------------------------
***************************************************************************/

void ESP_Core::setIsCreateIcon(bool c)
{
    this->isCreateIcon = c;
}

void ESP_Core::setIsAlert(bool a)
{
    this->alert = a;
}