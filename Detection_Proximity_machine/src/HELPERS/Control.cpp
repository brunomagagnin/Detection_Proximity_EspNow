
/*********************
 *      INCLUDES
 *********************/
#include <time.h>
#include "HELPERS/control.h"
#include "Config.h"
#include "ESP/ESP_Core.h"

/**********************
 *   GLOBAL VARIABLES
 **********************/
bool controlAlert = false;
uint32_t lastAlert = 0;

/**********************
 *   TIMER MSGBOX
 **********************/
time_t startTimeMbox = 0;
double setPointTimeMbox = 10.0;

/**********************
 *   TIMER ICON
 **********************/
time_t startTimeIcon = 0;
double setPointTimeIcon = 15.0;

/**********************
 *       ARRAY
 **********************/
bool iconCreate[NUMBER_OF_PERSON] = {false, false};

namespace controller
{
    void alertSoundVibration(bool b)
    {
        // Minimum time for new alert
        if (millis() - lastAlert > REFRESH)
        {
            if (!controlAlert)
            {
                controlAlert = b;
            }
        }

        // Turn on vibration and sound
        if (controlAlert)
        {
            lastAlert = millis();
            //M5.Axp.SetLDOEnable(3, true); //disebled
            for (int a = 0; a < 3; a++)
            {
                M5.Spk.DingDong();
            }
        }

        // Turn off vibration and sound
        if (controlAlert && millis() - lastAlert > VIBRATION_TIME)
        {
            //M5.Axp.SetLDOEnable(3, false); //disebled
            controlAlert = false;
        }
    }

    bool timeTocloseMbox(bool start)
    {
        if (start)
        {
            if (startTimeMbox == 0)
            {
                startTimeMbox = time(NULL);
            }

            time_t now = time(NULL);
            double current_time = difftime(now, startTimeMbox);

            if (current_time >= setPointTimeMbox)
            {
                startTimeMbox = 0;
                return true;
            }
            else
            {
                return false;
            }
        }
        else
        {
            startTimeMbox = 0;
            return false;
        }
    }
    bool timeToHideIcon(bool start)
    {
        if (start)
        {
            if (startTimeIcon == 0)
            {
                startTimeIcon = time(NULL);
            }

            time_t now = time(NULL);
            double current_time = difftime(now, startTimeIcon);

            if (current_time >= setPointTimeIcon)
            {
                startTimeIcon = 0;
                return true;
            }
            else
            {
                return false;
            }
        }
        else
        {
            startTimeIcon = 0;
            return false;
        }
    }
}