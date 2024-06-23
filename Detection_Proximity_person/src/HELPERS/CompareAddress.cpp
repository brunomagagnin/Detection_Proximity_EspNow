#include "HELPERS/CompareAddress.h"

namespace compare
{
    bool compareAddressPeer(Peer *obj, char *mac)
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
                c = false;
                break;
            }
        }
        if (c)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
}
