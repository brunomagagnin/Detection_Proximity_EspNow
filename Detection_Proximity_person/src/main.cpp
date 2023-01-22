#include <arduino.h>
#include <M5Core2.h>
#include <esp_wifi.h>
#include <esp_now.h>
#include <WiFi.h>
#include "..\lib/ESP_Peer.h"

// variaveis que indicam o núcleo
static uint8_t taskCoreZero = 0;
static uint8_t taskCoreOne = 1;

// PARAMETERS
#define CHANNEL 1
#define TARGET_ALERT -68
#define HYSTERESIS -10
#define TARGET_DANGER -10
#define MAX_MISSING_ALERT 10000
#define MAX_MISSING_DANGER 10000
#define REFRESH_ALERT 15000
#define REFRESH_DANGER 5000
#define TIME_TO_LEAVE 10000

#define TARGET_LEAVE -80
#define MAX_MISSING_TIME 10000
#define ALERT_TIME 1000
#define DANGER_TIME 3000
#define REFRESH 10000
#define ARRAY_SIZE 2

// Screens
#define SPACES 40
#define UPDATE_TIME 2000
bool controlScreen = true;
int cursor;

// Mac Address of peers  "0c:b8:15:ec:22:6c" 0c:f8:15:ic:23:7c
uint8_t peerMacAddress[6] = {0x0C, 0xB8, 0x15, 0xEC, 0x22, 0x6C};
char addressPeers[ARRAY_SIZE][18] = {"0c:b8:15:ec:22:6c", "0c:f8:15:ic:23:7c"};
String namePeer[ARRAY_SIZE] = {"Maquina 1  ", "Maquina 2  "};
bool markerOfPeerFound[ARRAY_SIZE] = {false, false};
uint32_t timeFoundPeer[ARRAY_SIZE] = {0, 0};
ESP_Peer *obj;
int rssiPeersArray[2][4];

// Control
bool controlAlert = false;
uint32_t lastAlert = 0;
uint32_t lastUpdate = 0;
uint32_t lastScanTime = 0;
uint32_t lastFoundTimePeer;
bool found = false;
int rssi;
int rssiPeers;
char macStr[18];

esp_now_peer_info_t peer;

/*----------------------------------------------------------------------
-------------------------------FUNCTIONS--------------------------------
----------------------------------------------------------------------*/

// When approaching
void alert(ESP_Peer *obj, uint32_t &lastAlert, bool &controlAlert)
{
    bool offAlert = false;
    if (millis() - lastAlert > REFRESH_ALERT)
    {
        if (!controlAlert)
        {
            for (int i = 0; i < ARRAY_SIZE; i++)
            {
                if (obj[i].getIsAlert())
                {
                    //Serial.println("Alerta ON!");
                    controlAlert = true;
                    break;
                }
            }
        }
    }
    if (controlAlert)
    {
        M5.Axp.SetLDOEnable(3, true);
        for (int a = 0; a < 3; a++)
        {
            M5.Spk.DingDong();
        }
        controlAlert = false;
        lastAlert = millis();
    }
    else
    {
        M5.Axp.SetLDOEnable(3, false);
    }
}
// When very close
void danger(bool &controlVibration)
{
    uint32_t timeVibration;
    if (controlVibration)
    {
        M5.Axp.SetLDOEnable(3, true);
        for (int a = 0; a < 6; a++)
        {
            M5.Spk.DingDong();
        }
        controlVibration = false;
        timeVibration = millis();
    }
    else if (!controlVibration && (millis() - timeVibration) > DANGER_TIME)
    {
        M5.Axp.SetLDOEnable(3, false);
    }
}

bool compareAddress(ESP_Peer *obj, char *mac, int index)
{
    bool c;
    for (int a = 0; a < 17; a++)
    {
        if (obj[index].getMacAddress(a) == mac[a])
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

void setStatePeers(ESP_Peer *obj)
{
    for (int a = 0; a < ARRAY_SIZE; a++)
    {
        if (obj[a].getIsAlert())
        {
            if (millis() - obj[a].getAlertTime() > MAX_MISSING_ALERT)
            {
                obj[a].setIsAlert(false);
                Serial.println("Saiu");
            }
        }
        if (millis() - obj[a].getDangerTime() > MAX_MISSING_DANGER)
        {
            obj[a].setIsDanger(false);
        }
    }
}

/*----------------------------------------------------------------------------------
------------------------------ ESPNOW - CALLBACKS AND FUNCTIONS --------------------
-----------------------------------------------------------------------------------*/
// Estrutura para calcular os pacotes e RSSI
typedef struct
{
    unsigned frame_ctrl : 16;
    unsigned duration_id : 16;
    uint8_t addr1[6]; // receiver address
    uint8_t addr2[6]; // sender address
    uint8_t addr3[6]; // filtering address
    unsigned sequence_ctrl : 16;
    uint8_t addr4[6]; // optional
} wifi_ieee80211_mac_hdr_t;

typedef struct
{
    wifi_ieee80211_mac_hdr_t hdr;
    uint8_t payload[0]; // network data ended with 4 bytes csum (CRC32)
} wifi_ieee80211_packet_t;

void promiscuous_rx_cb(void *buf, wifi_promiscuous_pkt_type_t type)
{
    // All espnow traffic uses action frames which are a subtype of the mgmnt frames so filter out everything else.
    if (type != WIFI_PKT_MGMT)
        return;

    const wifi_promiscuous_pkt_t *ppkt = (wifi_promiscuous_pkt_t *)buf;
    const wifi_ieee80211_packet_t *ipkt = (wifi_ieee80211_packet_t *)ppkt->payload;
    const wifi_ieee80211_mac_hdr_t *hdr = &ipkt->hdr;

    rssi = ppkt->rx_ctrl.rssi;
}

void InitESPNow()
{
    // Se a inicialização foi bem sucedida
    if (esp_now_init() == ESP_OK)
    {
        Serial.println("ESPNow Init Success");
    }
    // Se houve erro na inicialização
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
    // Copia o endereço do array para a estrutura
    memcpy(peer.peer_addr, peerMacAddress, 6);
    // Adiciona o slave
    esp_now_add_peer(&peer);
}

// Send value to peer with mac addresss specified
void send(const uint8_t *value, uint8_t *peerMacAddress)
{
    esp_err_t result = esp_now_send(peerMacAddress, value, sizeof(value));
    // Se o envio foi bem sucedido
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

    for (int i = 0; i < ARRAY_SIZE; i++)
    {
        if (compareAddress(obj, macStr, i))
        {
            if (rssi / 4 > TARGET_ALERT)
            {
                obj[i].setIsAlert(true);
                obj[i].setAlertTime(millis());
            }
            else if (rssi > TARGET_DANGER)
            {
                obj[i].setIsDanger(true);
                obj[i].setDangerTime(millis());
            }
        }
    }
}

void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
    // Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}

/* ------------------------------------------------------------------------
                             SCREENS
------------------------------------------------------------------------- */
void displayFunction(ESP_Peer *obj, String namePeer[2])
{
    bool alert = false;
    int allLines = 2;
    int line = 90;
    M5.Lcd.fillRect(15, 15, 290, 205, BLACK);

    // Screen Alert
    for (int i = 0; i < ARRAY_SIZE; i++)
    {
        if (obj[i].getIsAlert())
        {
            alert = true;
            M5.Lcd.setTextSize(2);
            M5.Lcd.setCursor(25, 40);
            M5.Lcd.print("Maquina proxima:");
            m5.Lcd.fillRect(30, line, 270, 25, BLACK);
            M5.Lcd.setCursor(30, line);
            M5.Lcd.print(namePeer[i]);
            M5.Lcd.setCursor(30, 60);
            // M5.Lcd.print(rssiPeers);
            line = line + SPACES;
            allLines -= 1;
        }
    }
    if (allLines > 0)
    {
        for (int i = allLines; i = 0; i--)
        {
            m5.Lcd.fillRect(10, line, 320, 25, BLACK);
            line = line + SPACES;
        }
    }

    // Screen Serching
    if (!alert)
    {
        M5.Lcd.setTextSize(3);
        M5.Lcd.setCursor(70, 100);
        M5.Lcd.print("RASTREANDO");
    }
}

void displayBattery()
{
    int maxVolts = 410; // Battery Max volts * 100
    int minVolts = 320; // Battery Min Volts * 100
    int batt = map(M5.Axp.GetBatVoltage() * 100, minVolts, maxVolts, 0, 10000) / 100.0;

    // Draw Battery bar(s) on the right side of the screen
    uint16_t clr = GREEN;
    for (int x = 9; x >= 0; x--)
    {
        if (x < 3)
            clr = RED;
        else if (x < 6)
            clr = YELLOW;
        M5.Lcd.fillRoundRect(314, (216 - (x * 24)), 6, 21, 2, (batt > (x * 10)) ? clr : BLACK);
        M5.Lcd.drawRoundRect(314, (216 - (x * 24)), 6, 21, 2, TFT_LIGHTGREY);
    }
}

void taskScreen(void *pvParameters)
{
    while (true)
    {
        displayFunction(obj, namePeer);
        displayBattery();
        delay(2100);
    }
}

void taskSetState(void *pvParameters)
{
    while (true)
    {
        setStatePeers(obj);
        alert(obj, lastAlert, controlAlert);
        delay(600);
    }
}

void setup()
{
    Serial.begin(115200);
    WiFi.channel(CHANNEL);
    WiFi.mode(WIFI_STA);

    InitESPNow();
    addPeer(peerMacAddress);
    esp_now_register_recv_cb(onDataRecv);
    esp_now_register_send_cb(onDataSent);
    esp_wifi_set_promiscuous(true);
    esp_wifi_set_promiscuous_rx_cb(&promiscuous_rx_cb);

    obj = (ESP_Peer *)malloc(sizeof(obj) * ARRAY_SIZE);

    for (int i = 0; i < ARRAY_SIZE; i++)
    {
        obj[i] = ESP_Peer(addressPeers[i]);
        Serial.println(i);
    }

    M5.begin();
    M5.Lcd.setBrightness(200);
    M5.Lcd.drawJpgFile(SD, "/Images/image.jpg");
    M5.Spk.begin();

    // M5.Spk.DingDong();
    Serial.println("Chegou");
    delay(5000);
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.fillRect(10, 10, 300, 215, WHITE);
    M5.Lcd.fillRect(15, 15, 290, 205, BLACK);

    xTaskCreatePinnedToCore(
        taskSetState,
        "taskSetState",
        10000,
        NULL,
        1,
        NULL,
        taskCoreOne);
    delay(500);

    xTaskCreatePinnedToCore(
        taskScreen,
        "taskScreen",
        10000,
        NULL,
        1,
        NULL,
        taskCoreOne);
    delay(500);
}

void loop()
{
    delay(10000);
}
