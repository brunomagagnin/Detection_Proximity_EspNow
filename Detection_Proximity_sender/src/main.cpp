#include <Arduino.h>
#include <M5Atom.h>
#include <esp_wifi.h>
#include <esp_now.h>
#include <WiFi.h>

#include "Config.h"

static uint8_t taskCoreZero = 0;
static uint8_t taskCoreOne = 1;
//24:D7:EB:39:01:A0   
//{0xD4, 0xD4, 0xDA, 0x5C, 0xAA, 0x8C}, {0x25, 0xD7, 0xEB, 0x39, 0x01, 0xA0} 24:D7:EB:39:01:A0
static uint8_t macSlaves[NUMBER_OF_PEERS][6] = {{0xD4, 0xD4, 0xDA, 0x5C, 0xAA, 0x8C}, {0x24, 0xD7, 0xEB, 0x39, 0x01, 0xA0}};
static uint8_t macBroadcast[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
static char addressPeers[NUMBER_OF_PEERS][18] = {"d4:d4:da:5c:aa:8c", "24:d7:eb:39:01:a0"};
static bool markerOfPeerFound[NUMBER_OF_PEERS] = {0, 0};

static int rssi;

esp_now_peer_info_t peer;

/*==================================================================================================
-----------------------------------------SENDING STATUS---------------------------------------------
====================================================================================================*/
static const uint8_t approachRequest = 201;
static const uint8_t acceptedApproximation = 202;
static const uint8_t notAuthorized = 203;
static const uint8_t broadCast = 255;

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
void broadcastSend()
{
    esp_err_t result = esp_now_send(macBroadcast, (uint8_t *)&broadCast, sizeof(broadCast));

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
    char macStr[18];
    // Copy address of sender
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
             mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
}

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
    char macStr[18];
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
             mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
}

void setup()
{
    WiFi.channel(CHANNEL);
    WiFi.mode(WIFI_STA);
    //esp_wifi_set_promiscuous(true);
    //esp_wifi_set_promiscuous_rx_cb(&promiscuous_rx_cb);
    esp_wifi_set_channel(CHANNEL, WIFI_SECOND_CHAN_NONE);
    Serial.begin(115200);
    Serial.print("MacAddress deste Dispositivo: ");
    Serial.println(WiFi.macAddress());

    delay(6000);

    InitESPNow();

    int slavesCount = sizeof(macSlaves) / 6 / sizeof(uint8_t);
    // for each slave
     Serial.println("Adicionando Slaves");
    for (int i = 0; i < slavesCount; i++)
    {
        esp_now_peer_info_t slave;
        slave.channel = CHANNEL;
        slave.ifidx = WIFI_IF_STA;
        slave.encrypt = 0;
        memcpy(slave.peer_addr, macSlaves[i], sizeof(macSlaves[i]));
        //esp_now_add_peer(&slave);
        if (esp_now_add_peer(&slave) != ESP_OK)
        {
            Serial.print("ESP_NOW: Houve um erro ao adicionar um slave: ");
            Serial.println(addressPeers[i]);
        } else {
            Serial.println("ESP_NOW: ADICIONADO SLAVE: ");
            Serial.println(addressPeers[i]);
        }
        esp_now_register_recv_cb(onDataRecv);
    }
    
    Serial.println("Adicionando Broadcast");
    esp_now_peer_info_t bCast;
    bCast.channel = CHANNEL;
    bCast.encrypt = 0;
    memcpy(bCast.peer_addr, macBroadcast, 6);
    if(esp_now_add_peer(&bCast) != ESP_OK){
        Serial.println("ESP_NOW: Houve um erro ao adicionar Broadcast");
    }
    Serial.println("Broadcast adicionado.");

    esp_now_register_send_cb(OnDataSent);
}

void loop()
{
    delay(1000);
    broadcastSend();
}
