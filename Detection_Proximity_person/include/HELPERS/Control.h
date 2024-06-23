#ifndef CONTROL_H
#define CONTROL_H

/*********************
 *      INCLUDES
 *********************/
#include <M5Core2.h>
#include "ESP/ESP_Peer.h"
#include "ESP/ESP_Core.h"

namespace controller
{
    void alertSoundVibration(ESP_Core *core[NUMBER_OF_MACHINES]);
    void timeToHideIcon(ESP_Core *core[NUMBER_OF_MACHINES]);
   // bool iconAlert(ESP_Core *core, bool accepted);
    
    #if EXTERN_SENDERS
    void setCoreAlert(ESP_Peer *peer[NUMBER_OF_MACHINES][NUMBER_OF_EXTERN_PEERS], ESP_Core *core[NUMBER_OF_MACHINES]);
    #endif
}

#endif