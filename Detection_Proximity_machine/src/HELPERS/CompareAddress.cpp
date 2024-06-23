#include "HELPERS/CompareAddress.h"

namespace compare
{
    bool addressPeer(ESP_Core *obj, char *mac)
    {   
        bool c;
        for (int a = 0; a < 17; a++)
        {
            if (obj->getMacAddress(a) == mac[a])
            {
                c = true;
            }
            else
            {
                return false;
            }
        }
        if (c)
        {
            return true;
        }
        return false;
    }
}
