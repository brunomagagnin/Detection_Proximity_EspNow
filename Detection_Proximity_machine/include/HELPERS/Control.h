#ifndef CONTROL_H
#define CONTROL_H

/*********************
 *      INCLUDES
 *********************/
#include "Config.h"
#include <M5Core2.h>
#include "ESP/ESP_Core.h"

namespace controller
{
    bool timeTocloseMbox(bool start);

    bool timeToHideIcon(bool start);

    void alertSoundVibration(bool b);
}

#endif