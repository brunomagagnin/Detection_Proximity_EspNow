#include "control.h"

/*++++++++++++++++++++++++  ALERT  +++++++++++++++++++++++
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
bool controlAlert = false;
uint32_t lastAlert = 0;
bool acceptedApproximation = false;

/*++++++++++++++++++++++++  ALERT  +++++++++++++++++++++++
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
bool iconCreate[NUMBER_OF_PEERS] = {false, false};

/*++++++++++++++++++++++++  TIMER  +++++++++++++++++++++++
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
bool initTimer = false;
uint32_t setTime = 0;
const uint32_t setPoint = 20000;

namespace controller
{
    void alert(ESP_Peer *peers, bool accepted) //
    {
        acceptedApproximation = accepted;

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
            if (!acceptedApproximation && controlAlert)
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
/*
    bool iconAlert(ESP_Peer *peers, bool accepted)
    {
        if (!iconCreate)
        {
            for (int index = 0; index < NUMBER_OF_PEERS; index++)
            {
                if (peers->getAcceptClose() && !iconCreate[index])
                {
                    return true;
                }
            }
        }
        return false;
    }
*/
    bool counterTime(ESP_Peer *peers)
    {
        if (!initTimer)
        {
            initTimer = true;
            setTime = millis();
        }

        if (millis() - setTime > setPoint)
        {
            setTime = millis();
            return false;
        }
        for (int i = 0; i < 2; i++)
        {
            peers->setAcceptClose(false);
        }
        return true;
    }
}