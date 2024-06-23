#ifndef CORESTATE_H
#define CORESTATE_H

#include <Arduino.h>

class CoreState
{

private:
    bool alert;
    bool acceptApproach;
    bool requestApproach;
    u_int32_t alertTime;
    

public:
    CoreState();

    bool getIsAlert();
    bool getRequestApproach();
    bool getAcceptApproach();

    u_int32_t getAlertTime();

    void setRequestApproach(bool request);
    void setAcceptApproach(bool accept);

    void setIsAlert(bool alert);
    void setAlertTime(u_int32_t time);
};

#endif
