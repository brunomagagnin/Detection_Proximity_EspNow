#ifndef MAC_ADDRESS_H
#define MAC_ADDRESS_H

#include <Config.h>
#include <Arduino.h>

#define EXTERN_SENDERS true;
/*+++++++++++++++++++++++++++++++++++++++++++++
          MAC ADDRESS FOR BROADCAST
+++++++++++++++++++++++++++++++++++++++++++++++*/
static uint8_t macBroadcast[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

/*+++++++++++++++++++++++++++++++++++++++++++++
        WHEN YOU NEED TO ONE SENDER
+++++++++++++++++++++++++++++++++++++++++++++++*/
#if EXTERN_SENDERS 
static uint8_t macSenders[NUMBER_OF_PEERS][6] = {{0xD2, 0xD1, 0xDA, 0x5C, 0xAA, 0x8C}, {0x24, 0xD7, 0xEB, 0x39, 0x01, 0xA0}};
static char addressSenders[NUMBER_OF_PEERS][18] = {"d2:d1:da:5c:aa:8c", "24:d7:eb:39:01:a0"};

#else
/*+++++++++++++++++++++++++++++++++++++++++++++
WHEN YOU NEED TO USE EXTERN SENDERS(TWO SENDERS)
+++++++++++++++++++++++++++++++++++++++++++++++*/
static uint8_t macSenders[NUMBER_OF_PEERS][6] = {{0xD4, 0xD4, 0xDA, 0x5C, 0xAA, 0x8C}, {0x24, 0xD7, 0xEB, 0x39, 0x01, 0xA0}};
static char addressSenders[NUMBER_OF_PEERS][18] = {"d4:d4:da:5c:aa:8c", "24:d7:eb:39:01:a0"};
#endif

#endif