#ifndef MAC_ADDRESS_H
#define MAC_ADDRESS_H

#include <Config.h>
#include <Arduino.h>

/*+++++++++++++++++++++++++++++++++++++++++++++
          MAC ADDRESS FOR BROADCAST
+++++++++++++++++++++++++++++++++++++++++++++++*/
static uint8_t macBroadcast[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

/*+++++++++++++++++++++++++++++++++++++++++++++
   WHEN YOU NEED TWO OR MORE EXTERN BEACONS 
+++++++++++++++++++++++++++++++++++++++++++++++*/
#if EXTERN_SENDERS 
//Extern senders
static uint8_t macSenders[NUMBER_OF_MACHINES][NUMBER_OF_EXTERN_PEERS][6] = {
                        {{0xD4, 0xD4, 0xDA, 0x9E, 0x32, 0x64}, {0xD4, 0xD4, 0xDA, 0x84, 0xF5, 0xB8}}, 
                        {{0xD3, 0xD3, 0xDA, 0x9E, 0x32, 0x64}, {0xD3, 0xD3, 0xDA, 0x84, 0xF5, 0xB8}}
                        };
static char addressSenders[NUMBER_OF_MACHINES][NUMBER_OF_EXTERN_PEERS][18] = {
                        {"d4:d4:da:9e:32:64", "d4:d4:da:84:f5:b8"}, 
                        {"d3:d3:da:9e:32:64", "d3:d3:da:84:f5:b8"}};
//Intern Peer
static uint8_t macMachine[NUMBER_OF_MACHINES][6] = {{0xD4, 0xD4, 0xDA, 0x5C, 0xAA, 0x8C}, {0x25, 0xD7, 0xEB, 0x39, 0x01, 0xA0}};
static char addressMachine[NUMBER_OF_MACHINES][18] = {"d4:d4:da:5c:aa:8c", "24:d7:eb:39:01:a0"};

#else
/*+++++++++++++++++++++++++++++++++++++++++++++
       WHEN YOU NEED TO USE ONE BEACON
+++++++++++++++++++++++++++++++++++++++++++++++*/
static uint8_t macSenders[NUMBER_OF_MACHINES][6] = {{0xD4, 0xD4, 0xDA, 0x5C, 0xAA, 0x8C}, {0x24, 0xD7, 0xEB, 0x39, 0x01, 0xA0}};
static char addressSenders[NUMBER_OF_MACHINES][18] = {"d4:d4:da:5c:aa:8c", "24:d7:eb:39:01:a0"};
//Intern Peer
static uint8_t macMachine[NUMBER_OF_MACHINES] = {0xD4, 0xD4, 0xDA, 0x5C, 0xAA, 0x8C}, {0x25, 0xD7, 0xEB, 0x39, 0x01, 0xA0};
static char addressMachine[NUMBER_OF_MACHINES] = {"d4:d4:da:5c:aa:8c", "24:d7:eb:39:01:a0"};
#endif


#endif