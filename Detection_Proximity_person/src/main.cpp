/*********************
 *      INCLUDES
 *********************/
#include <esp_wifi.h>
#include <esp_now.h>
#include <WiFi.h>

#include "Config.h"
#include <Mac_Address.h>
#include "HELPERS/CompareAddress.h"
#include "HELPERS/Control.h"

#include "Peer/AddPeer.h"
#include "ESP/ESP_Core.h"

#include <lvgl.h>
#include "GUI/Setup.h"
#include "GUI/GUI.h"

/**********************
 *  STATIC VARIABLES
 **********************/
static uint8_t taskCoreZero = 0;
static uint8_t taskCoreOne = 1;

/**********************
 *   OBJECT ARRAY
 **********************/
#if EXTERN_SENDERS
ESP_Core *core[NUMBER_OF_MACHINES];
ESP_Peer *peers[NUMBER_OF_MACHINES][NUMBER_OF_EXTERN_PEERS];

#else
ESP_Core *core[NUMBER_OF_MACHINES];
#endif

/**********************
 *   GLOBAL VARIABLES
 **********************/
int rssi;
char macStr[18];

/**********************
     SENDING STATUS
 **********************/
const uint8_t approachRequest = 201;
const uint8_t acceptedApproximation = 202;
const uint8_t notAuthorized = 203;

/*==================================================================================================
----------------------------------------- HANDLER LVGL ---------------------------------------------
====================================================================================================*/
void lv_handler()
{
    static uint32_t previousUpdate = 0;
    static uint32_t interval = 0;

    if (millis() - previousUpdate > interval)
    {
        previousUpdate = millis();
        uint32_t interval = lv_timer_handler(); // Update the UI
    }
}

/*====================================================================================================
======================================================================================================
------------------------------ ESPNOW - CALLBACKS AND FUNCTIONS --------------------------------------
======================================================================================================
======================================================================================================*/

// Struct to calc packags and rssi
typedef struct
{
    unsigned frame_ctrl : 16;
    unsigned duration_id : 16;
    uint8_t addr1[6]; // receiver address
    uint8_t addr2[6]; // sender address
    uint8_t addr3[6]; // filtering address
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

void send(const uint8_t *value, uint8_t *peerMacAddress)
{
    esp_err_t result = esp_now_send(peerMacAddress, value, sizeof(value));

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
            Serial.println("There is no exception.");
        }
    }
}

void onDataRecv(const uint8_t *mac_addr, const uint8_t *value, int len)
{
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
             mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);

#if EXTERN_SENDERS
    for (int a = 0; a < NUMBER_OF_MACHINES; a++)
    {
        if (compare::compareAddressPeer(core[a], macStr))
        {
            if (*value == acceptedApproximation)
            {
                core[a]->setAcceptApproach(true);
                break;
            }
            if (*value == notAuthorized)
            {
                core[a]->setAcceptApproach(false);
                break;
            }
        }

        for (int b = 0; b < NUMBER_OF_EXTERN_PEERS; b++)
        {
            if (compare::compareAddressPeer(peers[a][b], macStr))
            {
                if (rssi > TARGET_ALERT)
                {
                    peers[a][b]->setIsAlert(true);
                    peers[a][b]->setAlertTime(millis());
                }
            }
        }
    }

#else
    for (int a = 0; a < NUMBER_OF_MACHINES; a++)
    {
        if (compare::compareAddressPeer(core[a], macStr))
        {
            if (*value == acceptedApproximation)
            {
                core[a]->setAcceptApproach(true);
                break;
            }
            if (*value == notAuthorized)
            {
                core[a]->setAcceptApproach(false);
            }

            if (rssi > TARGET_ALERT)
            {
                core[a]->setIsAlert(true);
                core[a]->setAlertTime(millis());
            }
        }
    }
#endif
}

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
    char macStr[18];
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
             mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
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
    lv_handler();

#if EXTERN_SENDERS
    while (true)
    {
        for (int i = 0; i < 2; i++)
        {
            if (!core[i]->getIsCreateIcon() && core[i]->getAcceptApproach())
            {
                screen::create_alert(i);
                core[i]->setIsCreateIcon(true);
            }
        }

        for (int i = 0; i < 2; i++)
        {
            if (!core[i]->getAcceptApproach() && core[i]->getIsCreateIcon())
            {
                screen::delete_alert(i);
                core[i]->setIsCreateIcon(false);
            }
        }
        lv_handler();
        vTaskDelay(30);
    }
}

#else
    while (true)
    {
        for (int i = 0; i < 2; i++)
        {
            if (!core[i]->getIsCreateIcon() && core[i]->getAcceptApproach())
            {
                screen::create_alert(i);
                core[i]->setIsCreateIcon(true);
            }
        }

        for (int i = 0; i < 2; i++)
        {
            if (!core[i]->getAcceptApproach() && core[i]->getIsCreateIcon())
            {
                screen::delete_alert(i);
                core[i]->setIsCreateIcon(false);
            }
        }
        lv_handler();
        vTaskDelay(30);
    }
}
#endif

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

#if EXTERN_SENDERS
    AddPeer::addPeerSlave(macSenders);
    AddPeer::addMachineSlave(macMachine);
#endif

    AddPeer::addMachineSlave(macMachine);

    esp_now_register_recv_cb(onDataRecv);
    esp_now_register_send_cb(OnDataSent);

    while (true)
    {
        if (screen::getCallMachine())
        {
            send(&approachRequest, macMachine[screen::getCurrentTab()]);
            screen::turnOffCallMachine();
        }

#if EXTERN_SENDERS
        controller::setCoreAlert(peers, core);
#endif
        controller::alertSoundVibration(core);
        vTaskDelay(300);
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

#if EXTERN_SENDERS

    for (int a = 0; a < NUMBER_OF_MACHINES; a++)
    {
        for (int b = 0; b < NUMBER_OF_EXTERN_PEERS; b++)
        {
            peers[a][b] = new ESP_Peer(addressSenders[a][b]);
        }
    }
    for (int a = 0; a < NUMBER_OF_MACHINES; a++)
    {
        core[a] = new ESP_Core(addressMachine[a]);
    }
#else
    for (int a = 0; a < NUMBER_OF_MACHINES; a++)
    {
        core[a] = new ESP_Core(addressMachine[a]);
    }
#endif

    xTaskCreatePinnedToCore(
        taskScreen,
        "taskScreen",
        10000,
        NULL,
        2,
        NULL,
        taskCoreOne);
    delay(50);

    xTaskCreatePinnedToCore(
        espnowTask,
        "espnowTask",
        10000,
        NULL,
        2,
        NULL,
        taskCoreZero);
    delay(50);
}

void loop()
{
    delay(100000);
}
