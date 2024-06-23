
/*********************
 *      INCLUDES
 *********************/
#include "HELPERS/control.h"
#include "Config.h"


/**********************
 *   GLOBAL VARIABLES
 **********************/
bool controlAlert = false;
uint32_t lastAlert = 0;


/**********************
 *       ARRAY
 **********************/
bool iconCreate[NUMBER_OF_MACHINES] = {false, false};

namespace controller
{
    void alertSoundVibration(ESP_Core *core[NUMBER_OF_MACHINES]) 
    {
        // Minimum time for new alert
        if (millis() - lastAlert > REFRESH)
        {
            if (!controlAlert)
            {
                for (int i = 0; i < NUMBER_OF_MACHINES; i++)
                {
                    if (core[i]->getIsAlert() && !core[i]->getAcceptApproach())
                    {
                        controlAlert = true;
                        break;
                    }
                }
            }
            // Turn on vibration and sound
            if (controlAlert)
            {
                lastAlert = millis();
                M5.Axp.SetLDOEnable(3, true);
                for (int a = 0; a < 3; a++)
                {
                    M5.Spk.DingDong();
                }
            }
        }
        // Turn off vibration and sound
        if (controlAlert && millis() - lastAlert > VIBRATION_TIME)
        {
            M5.Axp.SetLDOEnable(3, false);
            controlAlert = false;
            for (int i = 0; i < NUMBER_OF_MACHINES; i++)
            {
                core[i]->setIsAlert(false);
            }
        }
    }

#if EXTERN_SENDERS
    void setCoreAlert(ESP_Peer *peer[NUMBER_OF_MACHINES][NUMBER_OF_EXTERN_PEERS], ESP_Core *core[NUMBER_OF_MACHINES])
    {
        for (int a = 0; a < NUMBER_OF_MACHINES; a++)
        {
            for (int b = 0; b < NUMBER_OF_EXTERN_PEERS; b++)
            {
                if (peer[a][b]->getIsAlert())
                {
                    core[a]->setIsAlert(true);
                    peer[a][b]->setIsAlert(false);
                }
            }
        }
    }
#endif
}