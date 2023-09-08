#include <Arduino.h>
#include <M5Core2.h>
#include <lvgl.h>
#include <esp_wifi.h>
#include <esp_now.h>
#include <WiFi.h>

#include "Config.h"
#include "ESP_PEER/ESP_Peer.h"

uint8_t macSlaves[NUMBER_OF_PEERS][6] = {{0x0C, 0xB8, 0x15, 0xEC, 0x22, 0x6C}, {0x24, 0xD7, 0xEB, 0x39, 0x01, 0xA0}};
uint8_t macBroadcast[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
char addressPeers[NUMBER_OF_PEERS][18] = {"0c:b8:15:ec:22:6c", "24:d7:eb:39:01:a0"};

ESP_Peer *peers;
esp_now_peer_info_t peer;
int rssi;

bool msgboxIsON = false;
bool msgBoxCreate = false;
uint32_t msgTime = 0;

bool callMachineActive = false;
bool responsePerson = false;
int indexResponse = 0;
uint8_t returnMachine = 203;

bool imageCreate = false;

uint8_t approachRequest = 201;
uint8_t acceptedApproximation = 202;
uint8_t notAuthorized = 203;
uint8_t broadCast = 255;

/*==================================================================================================
-------------------------------------------- PROTOTYPE ---------------------------------------------
====================================================================================================*/
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p);
void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data);
void lv_begin();
void lv_handler();
void tabview_screen();
void label_msg(String name);
void create_alert();
void delete_alert();
void create_company();
void delete_company();
static void event_m(lv_event_t *e);
void lv_msgbox(int index);
void lv_screen();
bool compareAddress(ESP_Peer *obj, char *mac, int index);
void promiscuous_rx_cb(void *buf, wifi_promiscuous_pkt_type_t type);
void InitESPNow();
void addPeer(uint8_t *peerMacAddress);
void onDataRecv(const uint8_t *mac_addr, const uint8_t *value, int len);
void broadcastSend();
void send(int index, uint8_t value);
void alert(ESP_Peer *peers, bool accepted);
bool counterTime();


/*==================================================================================================
====================================================================================================
--------------------------------------------  SCREEN  ----------------------------------------------
====================================================================================================
====================================================================================================*/

/*==================================================================================================
--------------------------------------------SETUP DISPLAY-------------------------------------------
====================================================================================================*/
static const uint16_t screenWidth = 320;
static const uint16_t screenHeight = 240;

TFT_eSPI tft = TFT_eSPI();
static lv_disp_draw_buf_t disp_buf;
static lv_color_t buf[screenWidth * screenHeight / 10];

// Display flushing
void my_disp_flush(lv_disp_drv_t *disp,
                   const lv_area_t *area, lv_color_t *color_p) 
{
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);
    tft.startWrite();
    tft.setAddrWindow(area->x1, area->y1, w, h);
    tft.pushColors(&color_p->full, w * h, true);
    tft.endWrite();
    lv_disp_flush_ready(disp);
}

// Read the touchpad
void my_touchpad_read(lv_indev_drv_t *indev_driver,
                      lv_indev_data_t *data)
{
    TouchPoint_t pos = M5.Touch.getPressPoint();
    bool touched = (pos.x == -1) ? false : true;
    if (!touched)
    {
        data->state = LV_INDEV_STATE_REL;
    }
    else
    {
        data->state = LV_INDEV_STATE_PR;
        data->point.x = pos.x;
        data->point.y = pos.y;
    }
}

void lv_begin()
{
    lv_init();
    lv_disp_draw_buf_init(&disp_buf, buf, NULL, screenWidth * screenHeight / 10);

    // Display
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = screenWidth;
    disp_drv.ver_res = screenHeight;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &disp_buf;
    lv_disp_drv_register(&disp_drv);

    // Initialize the input device driver
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = my_touchpad_read;
    lv_indev_drv_register(&indev_drv);
}

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

/*==================================================================================================
--------------------------------------------  VARIABLES  -------------------------------------------
====================================================================================================*/

LV_IMG_DECLARE(Icone);
LV_IMG_DECLARE(Logo);

// TabView
lv_obj_t *tabview;
lv_obj_t *tab1;

// Message Box
lv_obj_t *mbox;
lv_obj_t *eventMbox;
char macAdress[18];

// Image
lv_obj_t *imgTab;
lv_obj_t *imgLogo;
/*==================================================================================================
--------------------------------------------TABVIEW-------------------------------------------------
====================================================================================================*/

void tabview_screen()
{
    tabview = lv_tabview_create(lv_scr_act(), LV_DIR_TOP, 50);
    lv_obj_set_style_bg_color(tabview, lv_color_black(), 0);

    tab1 = lv_tabview_add_tab(tabview, "CENTRAL DE ALERTAS");

    lv_obj_t *tab_btns = lv_tabview_get_tab_btns(tabview);
    lv_obj_set_style_bg_color(tab_btns, lv_palette_darken(LV_PALETTE_GREY, 3), 0);
    lv_obj_set_style_text_color(tab_btns, lv_palette_lighten(LV_PALETTE_BLUE, 2), 0);
}

/*==================================================================================================
-------------------------------------------- LABEL -------------------------------------------------
====================================================================================================*/
void label_msg(String name)
{
    lv_obj_set_style_bg_color(tabview, lv_palette_darken(LV_PALETTE_RED, 4), 0);

    String cName = "#0000ff " + name + "#";
    int stringLength = cName.length() + 1;
    char char_array[stringLength];

    cName.toCharArray(char_array, stringLength);

    lv_obj_t *label;
    label = lv_label_create(tab1);

    lv_label_set_recolor(label, true);
    lv_label_set_text(label, char_array);
    lv_obj_set_style_text_font(label, LV_FONT_MONTSERRAT_10, 0);
    lv_obj_set_width(label, 150);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, -40);
}

/*==================================================================================================
-------------------------------------------- IMAGES -----------------------------------------------
====================================================================================================*/
void create_alert()
{
    imgTab = lv_img_create(tab1);
    lv_img_set_src(imgTab, &Icone);
    lv_obj_align(imgTab, LV_ALIGN_CENTER, 0, -15);
}

void delete_alert()
{
    lv_obj_del(imgTab);
}

void create_company()
{
    imgLogo = lv_img_create(lv_scr_act());
    lv_img_set_src(imgLogo, &Logo);
    lv_obj_align(imgLogo, LV_ALIGN_CENTER, 0, 0);
}

void delete_company()
{
    lv_obj_del(imgLogo);
}

/*==================================================================================================
------------------------------------------ MESSAGE BOX ---------------------------------------------
====================================================================================================*/
int indexAdress = 0;

static void event_m(lv_event_t *e)
{
    eventMbox = lv_event_get_current_target(e);
    LV_LOG_USER("Button %s clicked", lv_msgbox_get_active_btn_text(obj));
    String text = lv_msgbox_get_active_btn_text(eventMbox);
    if (text == "Aceitar")
    {
        indexResponse = indexAdress;
        returnMachine = acceptedApproximation;
        responsePerson = true;
        msgboxIsON = false;
        msgBoxCreate = false;
        lv_msgbox_close(mbox);
    }
    if (text == "Recusar")
    {
        indexResponse = indexAdress;
        returnMachine = notAuthorized;
        responsePerson = true;
        msgboxIsON = false;
        msgBoxCreate = false;
        lv_msgbox_close(mbox);
    }
}

void lv_msgbox(int index)
{
    indexAdress = index;
    Serial.println("Index passado na criação: ");
    Serial.println(indexAdress);
    msgTime = millis();

    static const char *btns[] = {"Aceitar", "Recusar", ""};
    mbox = lv_msgbox_create(NULL, " ", "Requisitado aproximacao de operador.", btns, false);
    lv_obj_add_event_cb(mbox, event_m, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_center(mbox);
}

/*==================================================================================================
---------------------------------------- CALL FUNCTION ---------------------------------------------
====================================================================================================*/

void lv_screen()
{
    tabview_screen();
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

    for (int i = 0; i < NUMBER_OF_PEERS; i++)
    {
        if (!msgboxIsON && *value == approachRequest && compareAddress(peers, macStr, i))
        {
            msgboxIsON = true;
            indexResponse = i;
        }
    }
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
-------------------------------------------------CONTROLS-----------------------------------------------------
==============================================================================================================
============================================================================================================== */

bool controlAlert = false;
uint32_t lastAlert = 0;

void alert(ESP_Peer *peers, bool accepted) //
{
    // Minimum time for new alert
    if (millis() - lastAlert > REFRESH)
    {
        // ALERTA
        if (accepted)
        {
            lastAlert = millis();
            for (int a = 0; a < 3; a++)
            {
                M5.Spk.DingDong();
            }
        }
    }
}

/*++++++++++++++++++++++++  TIMER  +++++++++++++++++++++++
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


const uint32_t setPoint = 7000;

bool counterTime()
{
    static bool initTimer = false;
    static uint32_t setTime = 0;

    if (!initTimer)
    {
        initTimer = true;
        setTime = millis();
    }

    if (millis() - setTime < setPoint)
    {
        return false;
    }
    initTimer = false;
    return true;
}

/*============================================================================================================
==============================================================================================================
---------------------------------------------------TASKS------------------------------------------------------
==============================================================================================================
============================================================================================================== */
static uint8_t taskCoreZero = 0;
static uint8_t taskCoreOne = 1;

void taskScreen(void *pvParameters)
{
    lv_begin();
    create_company();
    lv_handler();
    delay(6000);
    delete_company();
    lv_screen();

    while (true)
    {
        // Cria caixa de mensagem
        if (!msgBoxCreate && msgboxIsON)
        {
            lv_msgbox(indexResponse);
            msgBoxCreate = true;
        }
        // Tempo de caixa de mensagem aberta
        if (msgboxIsON && millis() - msgTime > CLOSE_MSGBOX)
        {
            if (mbox != NULL)
            {
                lv_msgbox_close(mbox);
                msgboxIsON = false;
                msgBoxCreate = false;
            }
        }

        if (!imageCreate && callMachineActive)
        {
            create_alert();
            imageCreate = true;
        }
        else if (imageCreate && !callMachineActive)
        {
            delete_alert();
            imageCreate = false;
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

    delay(7000);

    InitESPNow();

    int slavesCount = sizeof(macSlaves) / 6 / sizeof(uint8_t);
    // for each slave
    Serial.println("Adicionando Slaves: ");
    for (int i = 0; i < slavesCount; i++)
    {
        Serial.println(addressPeers[i]);
        esp_now_peer_info_t slave;
        slave.channel = CHANNEL;
        slave.encrypt = 0;
        memcpy(slave.peer_addr, macSlaves[i], sizeof(macSlaves[i]));
        esp_now_add_peer(&slave);
        esp_now_register_recv_cb(onDataRecv);
    }
    Serial.println("Adicionando Broadcast");
    esp_now_peer_info_t bCast;
    bCast.channel = CHANNEL;
    bCast.encrypt = 0;
    memcpy(bCast.peer_addr, macBroadcast, 6);
    esp_now_add_peer(&bCast);

    while (true)
    {
        if (!responsePerson)
        {
            broadcastSend();
        }
        else if (responsePerson)
        {
            if (returnMachine == acceptedApproximation)
            {
                callMachineActive = true;
                send(indexResponse, returnMachine);
            }
            else
            {
                responsePerson = false;
                send(indexAdress, notAuthorized);
            }
        }
        if (callMachineActive)
        {
                 
            if (counterTime())
            {
                callMachineActive = false;
                responsePerson = false;
            }
        }
        alert(peers, callMachineActive);
        vTaskDelay(200);
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
