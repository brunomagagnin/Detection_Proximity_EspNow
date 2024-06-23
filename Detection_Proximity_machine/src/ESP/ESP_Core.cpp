#include "ESP/ESP_Core.h"
/*************************************************************************
------------------------------- CONSTRUCTORS------------------------------
**************************************************************************/
ESP_Core::ESP_Core(){}
 
ESP_Core::ESP_Core(char *macAddress) :  Peer(macAddress), CoreState()
{
    this->isCreateIcon = false;
    this->controlAlert = false;
}

/************************************************************************
---------------------------------GET METHODS-----------------------------
*************************************************************************/

bool ESP_Core::getIsCreateIcon()
{
    return this->isCreateIcon;
}

/**************************************************************************
-----------------------------------SET METHODS-----------------------------
***************************************************************************/

void ESP_Core::setIsCreateIcon(bool c)
{
    this->isCreateIcon = c;
}

