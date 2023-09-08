#include <Arduino.h>
#include <M5Core2.h>
#include <lvgl.h>
#include <esp_wifi.h>
#include <esp_now.h>
#include <WiFi.h>

#include "Config.h"
#include "ESP_PEER/ESP_Peer.h"
#include "Control/control.h"
#include "GUI/setup.h"
#include "GUI/gui.h"

static uint8_t taskCoreZero = 0;
static uint8_t taskCoreOne = 1;

// static const uint8_t call = 204;
//  D4:D4:DA:5C:AA:8C dispositivo novo
//  24:D7:EB:39:01:A0 dispositivo antigo

static uint8_t macSlaves[NUMBER_OF_PEERS][6] = {{0xD4, 0xD4, 0xDA, 0x5C, 0xAA, 0x8C}, {0x25, 0xD7, 0xEB, 0x39, 0x01, 0xA0}};
static char addressPeers[NUMBER_OF_PEERS][18] = {"d4:d4:da:5c:aa:8c", "0c:f8:15:ic:23:7c"};
static String namePeer[NUMBER_OF_PEERS] = {"Maquina 1  ", "Maquina 2  "};
static bool markerOfPeerFound[NUMBER_OF_PEERS] = {0, 0};
static ESP_Peer *peers;
static int rssi;
static char macStr[18];

esp_now_peer_info_t peer;

bool callMachineActive = false;
bool imageCreate = false;

static const uint8_t approachRequest = 201;
static const uint8_t acceptedApproximation = 202;
static const uint8_t notAuthorized = 203;
static const uint8_t broadCast = 255;

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

/*==================================================================================================
====================================================================================================
--------------------------------------------FUNCTIONS-----------------------------------------------
====================================================================================================
====================================================================================================*/

bool compareAddress(ESP_Peer *obj, char *mac, int index)
{
    bool c;
    for (int a = 0; a < 17; a++)
    {
        if (peers[index].getMacAddress(a) == mac[a])
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

void addPeer(uint8_t *peerMacAddress)
{
    peer.channel = CHANNEL;
    peer.encrypt = 0;
    memcpy(peer.peer_addr, peerMacAddress, 6);
    // Add slave
    esp_now_add_peer(&peer);
}

// Send value to peer with mac addresss specified
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
            Serial.println("Not sure what happened");
        }
    }
}

// Callback receive data
void onDataRecv(const uint8_t *mac_addr, const uint8_t *value, int len)
{
    // Copy address of sender
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
             mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);

    for (int i = 0; i < NUMBER_OF_PEERS; i++)
    {
        if (compareAddress(peers, macStr, i))
        {
            if (*value == acceptedApproximation)
            {
                peers[i].setAcceptClose(true);
                return;
            }
            else
            {
                peers[i].setAcceptClose(false);
            }

            if (!rssi > TARGET_ALERT)
            {
                peers[i].setIsAlert(true);
                peers[i].setAlertTime(millis());
            }
        }
    }
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

    while (true)
    {
        for (int i = 0; i < 2; i++)
        {
            if (!peers[i].getIsCreateIcon() && peers[i].getAcceptClose())
            {
                screen::create_alert(i);
                peers[i].setIsCreateIcon(true);
                callMachineActive = true;
            }
        }

        for (int i = 0; i < 2; i++)
        {
            if (!peers[i].getAcceptClose() && peers[i].getIsCreateIcon())
            {
                screen::delete_alert(i);
                peers[i].setIsCreateIcon(false);
                callMachineActive = false;
            }
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

    Serial.print("MacAddress deste Dispositivo: ");
    Serial.println(WiFi.macAddress());

    delay(6000);

    InitESPNow();

    int slavesCount = sizeof(macSlaves) / 6 / sizeof(uint8_t);
    // for each slave
    for (int i = 0; i < slavesCount; i++)
    {
        esp_now_peer_info_t slave;
        slave.channel = CHANNEL;
        slave.encrypt = 0;
        memcpy(slave.peer_addr, macSlaves[i], sizeof(macSlaves[i]));
        esp_now_add_peer(&slave);
        esp_now_register_recv_cb(onDataRecv);
    }
    esp_now_register_send_cb(OnDataSent);

    while (true)
    {
        if (screen::getCallMachine())
        {
            send(&approachRequest, macSlaves[screen::lv_current_tab()]);
            screen::setCallMachine();
        }
        controller::alert(peers, callMachineActive);
        vTaskDelay(250);
    }
}
// static const uint8_t approachRequest = 201;
// static const uint8_t acceptedApproximation = 202;
// static const uint8_t notAuthorized = 203;

/* ==========================================================================================================
=============================================================================================================
---------------------------------------------------SETUP-----------------------------------------------------
=============================================================================================================
============================================================================================================*/
void setup()
{
    M5.begin();
    Serial.begin(115200);

    peers = new ESP_Peer[NUMBER_OF_PEERS];
    for (int i = 0; i < NUMBER_OF_PEERS; i++)
    {
        peers[i] = ESP_Peer(addressPeers[i]);
    }

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
    delay(10000);
}
