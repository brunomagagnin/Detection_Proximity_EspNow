#include "control.h"

bool controlAlert = false;
uint32_t lastAlert = 0;

namespace controller
{
    void alert(ESP_Peer *peers) //
    {
        // Tempo mínimo para novo alerta
        if (millis() - lastAlert > REFRESH)
        {
            if (!controlAlert)
            {
                for (int i = 0; i < NUMBER_OF_PEERS; i++)
                {
                    if (peers[i].getIsAlert())
                    {
                        controlAlert = true;
                        break;
                    }
                }
            }
            // ALERTA
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
        // DESLIGA VIBRAÇÃO
        if (controlAlert && millis() - lastAlert > VIBRATION_TIME)
        {
            M5.Axp.SetLDOEnable(3, false);
            controlAlert = false;
            for (int i = 0; i < NUMBER_OF_PEERS; i++)
            {
               peers[i].setIsAlert(false);
            }
        }
         
    }
   
}