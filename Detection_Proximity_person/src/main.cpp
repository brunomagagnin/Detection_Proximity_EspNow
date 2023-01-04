#include <string.h>
#include <M5Core2.h>
#include <esp_wifi.h>
#include <esp_now.h>
#include <WiFi.h>

//PARAMETERS
#define CHANNEL 1
#define TARGET_RSSI -68
#define MAX_MISSING_TIME 15000
#define ALERT_TIME 1000
#define DANGER_TIME 3000

// Screens
#define SPACES 40
#define UPDATE_TIME 2000
bool controlScreen = true;
int cursor;

// Mac Address of peers
uint8_t peerMacAddress[6] = {0x0C, 0xB8, 0x15, 0xEC, 0x22, 0x6C};
char addressPeers[2][18] = {"0c:b8:15:ec:22:6c", "c2:a9:c7:96:d8:7b"};
String namePeer[2] = {"Maquina 1  ", "Maquina 2  "};
bool markerOfFoundPeer[2] = {false, false};

// Control
bool controlVibration = true;
uint32_t now = 0;
uint32_t lastUpdate = 0;
uint32_t lastScanTime = 0;
uint32_t timeFoundPeer[2] = {0, 0};
uint32_t lastFoundTimePeer;
bool found = false;
int rssi;
int rssiPeers;

esp_now_peer_info_t peer;


//When approaching
void alert(bool controlVibration)
{
    uint32_t timeVibration;
    if (controlVibration)
    {
        M5.Axp.SetLDOEnable(3, true);
        M5.Axp.SetLDOEnable(3, true);
        for (int a = 0; a < 3; a++)
        {
            M5.Spk.DingDong();
        }
        controlVibration = false;
        timeVibration = millis();
    }
    else if (!controlVibration && (now - timeVibration) > ALERT_TIME)
    {
        M5.Axp.SetLDOEnable(3, false);
    }
}
//When very close
void danger(bool controVibration)
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
    else if (!controlVibration && (now - timeVibration) > DANGER_TIME)
    {
        M5.Axp.SetLDOEnable(3, false);
    }
}

bool compareAddress(char address[2][18], char mac[18])
{
    bool controler = false;
    int cont = 0;
    for (int a = 0; a <= 1; a++)
    {
        if (!controler)
        {
            for (int i = 0; i <= 17; i++)
            {
                if (address[a][i] != mac[i])
                {
                    controler = false;
                    break;
                }
                else
                {
                    controler = true;
                }
            }
        }
    }

    if (controler)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/*----------------------------------------------------------------------------------
                                ESPNOW - CALLBACKS AND FUNCTIONS
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

// Callback receve data
void onDataRecv(const uint8_t *mac_addr, const uint8_t *value, int len)
{
    char macStr[18];
    // Copia o endereço do enviador
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
             mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
    if (compareAddress(addressPeers, macStr) && rssi > TARGET_RSSI)
    {
        markerOfFoundPeer[0] = true;
        found = true;
        timeFoundPeer[0] = millis();
        lastScanTime = millis();
        rssiPeers = rssi;   
    }
}

// Função que serve de callback para nos avisar
// sobre a situação do envio que fizemos
void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
    // Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}

/* ------------------------------------------------------------------------
                          DISPLAY AND SCREENS  
------------------------------------------------------------------------- */

void screenFoundPeer(bool markerOfFoundPeer[2], String namePeer[2])
{
    int allLines = 2;
    int line = 90;
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(25, 40);
    M5.Lcd.fillRect(15, 15, 290, 205, BLACK);
    M5.Lcd.print("Maquina proxima:");

    for (int index = 0; index < sizeof(markerOfFoundPeer); index++)
    {
        if (markerOfFoundPeer[index])
        {
            m5.Lcd.fillRect(30, line, 270, 25, BLACK);
            M5.Lcd.setCursor(30, line); 
            M5.Lcd.print(namePeer[index]);
            M5.Lcd.setCursor(30, 60);
            M5.Lcd.print(rssiPeers);
            line = line + SPACES;
            allLines -= 1;
        }
    }
    if (allLines > 0)
    {
        for (int index = allLines; index = 0; index--)
        {
            m5.Lcd.fillRect(10, line, 320, 25, BLACK);
            line = line + SPACES;
        }
    }
}

void screenSerchingPeer()
{
    M5.Lcd.fillRect(15, 15, 290, 205, BLACK);
    M5.Lcd.setTextSize(3);
    M5.Lcd.setCursor(70, 100);
    M5.Lcd.print("RASTREANDO");
}

void displayBattery()
{
    // M5.Lcd.setTextSize(1);
    // M5.Lcd.setFreeFont(FSS9);
    int maxVolts = 410; // Battery Max volts * 100
    int minVolts = 320; // Battery Min Volts * 100
                        // M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);

    // M5.Lcd.setTextSize(1);
    // char battInfo[5];
    // dtostrf(M5.Axp.GetBatVoltage(),1,2,battInfo);
    // String btInfo = "Batt: " + String(battInfo);
    // M5.Lcd.setTextDatum(BL_DATUM);
    // M5.Lcd.drawString(btInfo, 220, 215, GFXFF);
    //  drawDatumMarker(230,215);

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

    M5.begin();
    M5.Lcd.setBrightness(200);
    M5.Lcd.drawJpgFile(SD, "/Images/image.jpg");
    M5.Spk.begin();

    delay(200);
    M5.Spk.DingDong();

    delay(3000);
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.fillRect(10, 10, 300, 215, WHITE);
    M5.Lcd.fillRect(15, 15, 290, 205, BLACK);
}

void loop()
{
    now = millis();

    if (now - lastUpdate > UPDATE_TIME)
    {
        if (found &&  now - lastScanTime < MAX_MISSING_TIME)
        {
            screenFoundPeer(markerOfFoundPeer, namePeer);
            lastUpdate = millis();
            alert(controlVibration);
        }
        else
        {
            screenSerchingPeer();
            lastUpdate = millis();
            controlVibration = true;
            found = false;
        }
    }
    displayBattery();
}