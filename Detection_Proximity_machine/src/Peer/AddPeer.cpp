#include "Peer/AddPeer.h"

namespace AddPeer
{

    void addPersonSlave(uint8_t macSlaves[NUMBER_OF_PERSON][6])
    {
        Serial.println("ADDING SLAVES OF SENDERS");
        int slavesCount = sizeof(macSlaves) / 6 / sizeof(uint8_t);
        // for each slave
        for (int a = 0; a < NUMBER_OF_PERSON; a++)
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

    void addBroadcast()
    {
        uint8_t macBroadcast[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

        esp_now_peer_info_t bCast;
        bCast.channel = CHANNEL;
        bCast.encrypt = 0;
        memcpy(bCast.peer_addr, macBroadcast, 6);
        if (esp_now_add_peer(&bCast) != ESP_OK)
        {
            Serial.println("ESP_NOW: Houve um erro ao adicionar Broadcast");
        }
        Serial.println("Broadcast adicionado.");
    }
}