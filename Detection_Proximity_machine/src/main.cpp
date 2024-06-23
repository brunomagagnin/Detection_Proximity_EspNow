#include <Arduino.h>
#include <M5Core2.h>
#include <lvgl.h>
#include <esp_wifi.h>
#include <esp_now.h>
#include <WiFi.h>

#include "Config.h"
#include "Mac_Address.h"
#include "GUI/Setup.h"
#include "GUI/GUI.h"

#include "ESP/ESP_Core.h"
#include "Peer/Peer.h"
#include "ESP/CoreState.h"
#include "Peer/AddPeer.h"

#include "HELPERS/Control.h"
#include "HELPERS/CompareAddress.h"

ESP_Core *core[NUMBER_OF_PERSON];
esp_now_peer_info_t peer;

static uint8_t taskCoreZero = 0;
static uint8_t taskCoreOne = 1;

int rssi;

uint8_t approachRequest = 201;
uint8_t acceptedApproximation = 202;
uint8_t notAuthorized = 203;

/*==================================================================================================
====================================================================================================
--------------------------------------------  SCREEN  ----------------------------------------------
====================================================================================================
====================================================================================================*/

void lv_handler()
{
    static uint32_t previousUpdate = 0;
    static uint32_t interval = 0;

    if (millis() - previousUpdate > interval)
    {
        previousUpdate = millis();
        uint32_t interval = lv_timer_handler();
    }
}

/*=======================================================================================================
=========================================================================================================
------------------------------ ESPNOW - CALLBACKS AND FUNCTIONS ----------------------------------------
=========================================================================================================
=========================================================================================================*/

// Struct to calc packags and rssi
typedef struct
{
    unsigned frame_ctrl : 16;
    unsigned duration_id : 16;
    uint8_t addr1[6];
    uint8_t addr2[6];
    uint8_t addr3[6];
    unsigned sequence_ctrl : 16;
    uint8_t addr4[6];
} wifi_ieee80211_mac_hdr_t;

typedef struct
{
    wifi_ieee80211_mac_hdr_t hdr;
    uint8_t payload[0];
} wifi_ieee80211_packet_t;

void promiscuous_rx_cb(void *buf, wifi_promiscuous_pkt_type_t type)
{
    if (type != WIFI_PKT_MGMT)
        return;

    const wifi_promiscuous_pkt_t *ppkt = (wifi_promiscuous_pkt_t *)buf;
    const wifi_ieee80211_packet_t *ipkt = (wifi_ieee80211_packet_t *)ppkt->payload;
    const wifi_ieee80211_mac_hdr_t *hdr = &ipkt->hdr;

    rssi = ppkt->rx_ctrl.rssi;
}

void InitESPNow()
{
    if (esp_now_init() == ESP_OK)
    {
        Serial.println("ESPNow Init Success");
    }
    else
    {
        Serial.println("ESPNow Init Failed");
        ESP.restart();
    }
}

void addPeer(uint8_t *peerMacAddress)
{
    peer.channel = CHANNEL;
    peer.encrypt = 0;
    memcpy(peer.peer_addr, peerMacAddress, 6);
    // Add slave
    esp_now_add_peer(&peer);
}

// Callback receive data
void onDataRecv(const uint8_t *mac_addr, const uint8_t *value, int len)
{
    char macStr[18];
    // Copy address of sender
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
             mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);

    if (!screen::getIsCreatMsgbox())
    {
        for (int a = 0; a < NUMBER_OF_PERSON; a++)
        {
            if (compare::addressPeer(core[a], macStr))
            {
                if (*value == approachRequest)
                {
                    core[a]->setRequestApproach(true);
                    break;
                }
            }
        }
    }
}

void broadcastSend()
{
    esp_err_t result = esp_now_send(macBroadcast, (uint8_t *)&notAuthorized, sizeof(notAuthorized));

    if (DEBUG)
    {
        if (result == ESP_OK)
        {
            Serial.println("Success");
        }
        else if (result == ESP_ERR_ESPNOW_NOT_INIT)
        {
            Serial.println("ESPNOW not Init.");
        }
        else if (result == ESP_ERR_ESPNOW_ARG)
        {
            Serial.println("Invalid Argument");
        }
        else if (result == ESP_ERR_ESPNOW_INTERNAL)
        {
            Serial.println("Internal Error");
        }
        else if (result == ESP_ERR_ESPNOW_NO_MEM)
        {
            Serial.println("ESP_ERR_ESPNOW_NO_MEM");
        }
        else if (result == ESP_ERR_ESPNOW_NOT_FOUND)
        {
            Serial.println("Peer not found.");
        }
        else
        {
            Serial.println("Not sure what happened");
        }
    }
}

void send(int index, uint8_t value)
{
    esp_err_t result = esp_now_send(macSlaves[index], (uint8_t *)&value, sizeof(value));

    if (DEBUG)
    {
        if (result == ESP_OK)
        {
            Serial.println("Success");
        }
        else if (result == ESP_ERR_ESPNOW_NOT_INIT)
        {
            Serial.println("ESPNOW not Init.");
        }
        else if (result == ESP_ERR_ESPNOW_ARG)
        {
            Serial.println("Invalid Argument");
        }
        else if (result == ESP_ERR_ESPNOW_INTERNAL)
        {
            Serial.println("Internal Error");
        }
        else if (result == ESP_ERR_ESPNOW_NO_MEM)
        {
            Serial.println("ESP_ERR_ESPNOW_NO_MEM");
        }
        else if (result == ESP_ERR_ESPNOW_NOT_FOUND)
        {
            Serial.println("Peer not found.");
        }
        else
        {
            Serial.println("Not sure what happened");
        }
    }
}

/*============================================================================================================
==============================================================================================================
---------------------------------------------------TASKS------------------------------------------------------
==============================================================================================================
============================================================================================================== */

void taskScreen(void *pvParameters)
{
    setupLvgl::lv_begin();
    screen::create_company();
    lv_handler();
    delay(6000);
    screen::delete_company();
    screen::lv_screen();

    while (true)
    {
        if (!screen::getIsCreatMsgbox() && !screen::getIsCreatIcon())
        {
            for (int a = 0; a < NUMBER_OF_PERSON; a++)
            {
                if (core[a]->getRequestApproach())
                {
                    core[a]->setRequestApproach(false);
                    screen::lv_open_msgbox(a, core[a]);
                    break;
                }
            }
        }

        if (controller::timeTocloseMbox(screen::getIsCreatMsgbox()))
        {
            if (screen::lv_msgboxObj() != NULL)
            {
                screen::lv_msgboxClose();
            }
        }

        if (controller::timeToHideIcon(screen::getAcceptApprouch()))
        {
            screen::delete_alert();
        }

        lv_handler();
        vTaskDelay(30);
    }
}

void espnowTask(void *pvParameters)
{
    WiFi.channel(CHANNEL);
    WiFi.mode(WIFI_STA);

    esp_wifi_set_promiscuous(true);
    esp_wifi_set_promiscuous_rx_cb(&promiscuous_rx_cb);
    esp_wifi_set_channel(CHANNEL, WIFI_SECOND_CHAN_NONE);

    Serial.print("MacAddress of this device: ");
    Serial.println(WiFi.macAddress());

    delay(6000);

    InitESPNow();

    AddPeer::addPersonSlave(macSlaves);
    AddPeer::addBroadcast();

    esp_now_register_recv_cb(onDataRecv);

    while (true)
    {
        if (screen::getAcceptApprouch())
        {
            send(screen::getIndexCoreRequest(), acceptedApproximation);
        }
        else
        {
            broadcastSend();
        }

        controller::alertSoundVibration(screen::getAcceptApprouch());
        delay(300);
    }
}

/* ==========================================================================================================
=============================================================================================================
---------------------------------------------------SETUP-----------------------------------------------------
=============================================================================================================
============================================================================================================*/
void setup()
{
    M5.begin();
    Serial.begin(115200);

    for (int a = 0; a < NUMBER_OF_PERSON; a++)
    {
        core[a] = new ESP_Core(addressPeers[a]);
    }

    xTaskCreatePinnedToCore(
        taskScreen,
        "taskScreen",
        9000,
        NULL,
        2,
        NULL,
        taskCoreOne);
    delay(50);

    xTaskCreatePinnedToCore(
        espnowTask,
        "espnowTask",
        8000,
        NULL,
        2,
        NULL,
        taskCoreZero);
    delay(50);
}

void loop()
{
    delay(10000);
}
