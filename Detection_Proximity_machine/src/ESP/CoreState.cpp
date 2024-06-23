#include "ESP/CoreState.h"

/*************************************************************************
------------------------------- CONSTRUCTORS------------------------------
**************************************************************************/

CoreState::CoreState()
{
    this->alert = false;
    this->acceptApproach = false;
    this->requestApproach = false;
    this->alertTime = 0;
}

/************************************************************************
---------------------------------GET METHODS-----------------------------
*************************************************************************/

bool CoreState::getIsAlert()
{
    return this->alert;
}

bool CoreState::getRequestApproach()
{
    return this->requestApproach;
}

bool CoreState::getAcceptApproach(){
    return this->acceptApproach;
}

u_int32_t CoreState::getAlertTime()
{
    return this->alertTime;
}

/**************************************************************************
-----------------------------------SET METHODS-----------------------------
***************************************************************************/

void CoreState::setRequestApproach(bool request)
{
    this->requestApproach = request;
}

void CoreState::setAcceptApproach(bool accept)
{
    this->acceptApproach = accept;
}

void CoreState::setIsAlert(bool alert)
{
    this->alert = alert;
}

void CoreState::setAlertTime(u_int32_t time)
{
    this->alertTime = time;
}
