#include <string.h>
#include <M5Core2.h>
#include <esp_wifi.h>
#include <esp_now.h>
#include <WiFi.h>

#define CHANNEL 1
#define TARGET_RSSI -70
#define MAX_MISSING_TIME 5000

// Screens
#define SPACES 40
#define UPDATE_TIME 1000

// Mac Address dos peers
uint8_t peerMacAddress[6] = {0x0C, 0xB8, 0x15, 0xEC, 0x22, 0x6C};
char addressPeers[2][18] = {"0c:b8:15:ec:22:6c", "c2:a9:c7:96:d8:7b"};
String namePeer[2] = {"Colaborador 1  ", "Colaborador 2  "};
bool markerOfFoundPeer[2] = {false, false};
// Controle
bool controlVibration = false;
uint32_t now = 0;
uint32_t counter = 0;
uint32_t timeFoundPeer[2] = {0, 0};
int cursor;
int lastFoundTimePeer;
uint32_t lastUpdate = 0;
uint32_t lastScanTime = 0;
bool atualizaProcurar = false;
boolean found = false;

esp_now_peer_info_t peer;
int rssi;
int rssiPeers;
uint32_t pas = 0;

void vibrate(uint32_t time)
{
    
    if (now - time < 2000){
        M5.Axp.SetLDOEnable(3, true);
    }else{
        M5.Axp.SetLDOEnable(3, false);
    }
}

bool compareAddress(char address[2][18], char mac[18])
{
    bool controler = false;
    char addressPeers[2][18] = {"0C:B8:15:EC:22:6c", "c2:a9:c7:96:d8:7b"};
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

/* *********************************************************************************
   *********************************************************************************
    -------------------------  CALLBACKS ESPNOW  ----------------------------------
   *********************************************************************************
   **********************************************************************************/
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

// Função de inicialização do ESPNow
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

// Envia o valor para o peer que tenha o mac address especificado
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

// Callback recebe dados
void onDataRecv(const uint8_t *mac_addr, const uint8_t *value, int len)
{
    char macStr[18];
    // Copia o endereço do enviador
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
             mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
    if (compareAddress(addressPeers, macStr))
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

void screenFoundPeer(bool markerOfFoundPeer[2], String namePeer[2])
{
    int allLines = 2;
    int line = 40;
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(10, 20);
    M5.Lcd.drawRoundRect(20, 20, 280, 200, 2, BLACK);
    M5.Lcd.print("Quem esta proximo:");

    for (int index = 0; index < sizeof(markerOfFoundPeer); index++)
    {
        if (markerOfFoundPeer[index])
        {
            m5.Lcd.fillRect(10, line, 320, 25, BLACK);
            M5.Lcd.setCursor(10, line);
            M5.Lcd.print(namePeer[index]);
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
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(50, 100);
    M5.Lcd.print("RASTREANDO...");
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
    // M5.Spk.DingDong();
    delay(200);
    M5.Spk.DingDong();
    delay(1000);
    M5.Lcd.fillScreen(BLACK);
    // 235, 315   220 300
    M5.Lcd.drawRoundRect(10, 12, 300, 215, 2, WHITE);
    M5.Lcd.drawRoundRect(20, 20, 280, 200, 2, BLACK);

    delay(10000);
}

void loop()
{
    now = millis();
    if (now - lastUpdate > UPDATE_TIME)
    {
        if (now - lastScanTime < MAX_MISSING_TIME)
        {
            M5.Lcd.clearDisplay();
            screenFoundPeer(markerOfFoundPeer, namePeer);
            lastUpdate = millis();
        }
        else
        {
            M5.Lcd.clearDisplay();
            screenSerchingPeer();
            found = false;
            lastUpdate = millis();
        }
    }
}

// if (now - 1000 > pas)
//{
//     pas = now;
//     M5.Lcd.fillRect(10, 40, 320, 25, BLACK);
//     M5.Lcd.setCursor(10, 40);
//     M5.Lcd.print("Tempo:");
//     now = millis();
//     M5.Lcd.print(now + 100 - lastScanTime);
//     M5.Lcd.fillRect(10, 70, 320, 25, BLACK);
//     M5.Lcd.setCursor(10, 70);
//     M5.Lcd.print("Sinal:");
//     M5.Lcd.print(rssi);
//  }
//}

/*
if (found == true && rssiPeers > TARGET_RSSI)
    {
        Serial.println("Passou aqui");
        Serial.println(rssiPeers);
        found = false;
        cursor = 10;
        counter = 0;
        for (int i = 0; i < sizeof(markerOfFoundPeer); i++)
        {
            if (markerOfFoundPeer[i])
            {
                M5.Lcd.setCursor(10, cursor);
                m5.Lcd.fillRect(10, cursor, 320, 25, BLACK);
                M5.Lcd.println(namePeer[i]);
                if (i == 1 && counter == 0)
                {
                    m5.Lcd.fillRect(10, 60, 320, 25, BLACK);
                }
                cursor = cursor + 50;
                counter++;
            }
        }
        if (counter == 1 && markerOfFoundPeer[0])
        {
            m5.Lcd.fillRect(10, 60, 320, 25, BLACK);
        }
    }
    else if (now - lastFoundTimePeer > MAX_MISSING_TIME)
    {
        M5.Lcd.clearDisplay();
        M5.Lcd.setCursor(10, 60);
        M5.Lcd.print("Procurando...");
    }

    for (int i = 0; i < sizeof(markerOfFoundPeer); i++)
    {
        if (now - timeFoundPeer[i] > MAX_MISSING_TIME)
        {
            markerOfFoundPeer[i] = false;
        }
        else
        {
            markerOfFoundPeer[i] = true;
        }

**************ULTIMO RETIRADO**************************
 if (found == true && rssiPeers > TARGET_RSSI)
    {
        found = false;
        cursor = 10;
        counter = 0;
        for (int i = 0; i < sizeof(markerOfFoundPeer); i++)
        {
            if (markerOfFoundPeer[i])
            {
                M5.Lcd.setCursor(10, cursor);
                m5.Lcd.fillRect(10, cursor, 320, 25, BLACK);
                M5.Lcd.println(namePeer[i]);
                if (i == 1 && counter == 0)
                {
                    m5.Lcd.fillRect(10, 60, 320, 25, BLACK);
                }
                cursor = cursor + 50;
                counter++;
            }
        }
        if (counter == 1 && markerOfFoundPeer[0])
        {
            m5.Lcd.fillRect(10, 60, 320, 25, BLACK);
        }
    }
    else if (now - lastFoundTimePeer > MAX_MISSING_TIME)
    {
        M5.Lcd.clearDisplay();
        M5.Lcd.setCursor(10, 60);
        M5.Lcd.print("Procurando...");
    }

    for (int i = 0; i < sizeof(markerOfFoundPeer); i++)
    {
        if (now - timeFoundPeer[i] > MAX_MISSING_TIME)
        {
            markerOfFoundPeer[i] = false;
        }
        else
        {
            markerOfFoundPeer[i] = true;
        }
    }
*/