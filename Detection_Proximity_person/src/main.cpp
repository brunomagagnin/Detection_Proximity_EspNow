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

// B4:E6:2D:96:28:11 ESP32 PURO
//  0C:B8:15:EC:22:6C esp32 puro maior
//  Mac Address of peers  "0c:b8:15:ec:22:6c" 0c:f8:15:ic:23:7c
uint8_t peerMacAddress[6] = {0x0C, 0xB8, 0x15, 0xEC, 0x22, 0x6C};
char addressPeers[NUMBER_OF_PEERS][18] = {"0c:b8:15:ec:22:6c", "0c:f8:15:ic:23:7c"};
String namePeer[NUMBER_OF_PEERS] = {"Maquina 1  ", "Maquina 2  "};
bool markerOfPeerFound[NUMBER_OF_PEERS] = {0, 0};
ESP_Peer *peers;
int rssi;
char macStr[18];

esp_now_peer_info_t peer;

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
    if (result == ESP_OK)
    {
        Serial.println("Success");
    }
    else
    {
        Serial.println("Error");
    }
}

// Callback receive data
void onDataRecv(const uint8_t *mac_addr, const uint8_t *value, int len)
{
    // Copy address of sender
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
             mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);

    Serial.println("Peer detectado:");
    Serial.println(macStr);
    Serial.println(rssi);
    
    for (int i = 0; i < NUMBER_OF_PEERS; i++)
    {
        if (compareAddress(peers, macStr, i))
        {
            if (rssi > TARGET_ALERT)
            {
                peers[i].setIsAlert(true);
                peers[i].setAlertTime(millis());
            }
            /*
            if (rssi > TARGET_DANGER)
            {
                peers[i].setIsDanger(true);
                peers[i].setDangerTime(millis());
            }
            */
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
    screen::lv_screen();

    while (true)
    {
        lv_handler();
        vTaskDelay(30);
    }
}

void espnowTask(void *pvParameters)
{
    WiFi.channel(CHANNEL);
    WiFi.mode(WIFI_STA);

    InitESPNow();
    addPeer(peerMacAddress);
    esp_now_register_recv_cb(onDataRecv);
    // esp_now_register_send_cb(onDataSent);
    esp_wifi_set_promiscuous(true);
    esp_wifi_set_promiscuous_rx_cb(&promiscuous_rx_cb);

    while (true)
    {
        controller::alert(peers);
        vTaskDelay(500);
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
        1);
    delay(50);

    xTaskCreatePinnedToCore(
        espnowTask,
        "espnowTask",
        10000,
        NULL,
        2,
        NULL,
        0);
    delay(50);
}

void loop()
{
    delay(10000);
}
