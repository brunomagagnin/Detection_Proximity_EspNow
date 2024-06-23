#include "Peer/AddPeer.h"

namespace AddPeer
{
    void addPeerSlave(uint8_t macSlaves[NUMBER_OF_MACHINES][NUMBER_OF_EXTERN_PEERS][6])
    {
        Serial.println("ADDING SLAVES OF SENDERS");
        int slavesCount = sizeof(macSlaves[NUMBER_OF_MACHINES]) / 6 / sizeof(uint8_t);
        for (int a = 0; a < NUMBER_OF_MACHINES; a++)
        {
            for (int b = 0; b < slavesCount; b++)
            {
                esp_now_peer_info_t slave;
                slave.channel = CHANNEL;
                slave.encrypt = 0;
                memcpy(slave.peer_addr, macSlaves[a][b], sizeof(macSlaves[a][b]));
                if (esp_now_add_peer(&slave) != ESP_OK)
                {
                    Serial.print("ESP_NOW: There was an error adding a slave.");
                    
                }
                else
                {
                    Serial.println("ESP_NOW: ADD SLAVE.");
                }
            }
        }
    }

    void addMachineSlave(uint8_t macSlaves[NUMBER_OF_MACHINES][6])
    {
        Serial.println("ADDING SLAVES OF SENDERS");
        int slavesCount = sizeof(macSlaves) / 6 / sizeof(uint8_t);
        // for each slave
        for (int a = 0; a < NUMBER_OF_MACHINES; a++)
        {
            esp_now_peer_info_t slave;
            slave.channel = CHANNEL;
            slave.encrypt = 0;
            memcpy(slave.peer_addr, macSlaves[a], sizeof(macSlaves[a]));
            
            if (esp_now_add_peer(&slave) != ESP_OK)
            {
                Serial.print("ESP_NOW: There was an error adding a slave: " + a);
            }
            else
            {
                Serial.println("ESP_NOW: ADD SLAVE: " + a);
            }
        }
    }
}