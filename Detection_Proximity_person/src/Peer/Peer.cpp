#include "Peer/Peer.h"
/*----------------------------------------------------------------------------
----------------------------------- CONSTRUCTORS------------------------------
----------------------------------------------------------------------------*/

Peer::Peer(){}

Peer::Peer(char *macAddress)
{
    this->index = 0;
    this->rssi = 0;
 
    for (int i = 0; i < sizeof(this->macAddress); i++)
    {
        this->macAddress[i] = macAddress[i];
    }
}

/*----------------------------------------------------------------------------
--------------------------------------GET METHODS-----------------------------
----------------------------------------------------------------------------*/

char Peer::getMacAddress(int index)
{
    return this->macAddress[index];
}

char *Peer::getMacAddress()
{
    return this->macAddress;
}

int Peer::getRssi()
{
    return rssi;
}

int Peer::getRssiArray(int index)
{
    return rssiArray[index];
}


/*----------------------------------------------------------------------------
--------------------------------------SET METHODS-----------------------------
----------------------------------------------------------------------------*/

void Peer::setRssi(int rssi)
{
    this->rssi = rssi;
    this->rssiArray[this->index] = rssi;
    if (this->index == 3)
    {
        index = 0;
    }
    else
    {
        this->index = this->index + 1;
    }
}