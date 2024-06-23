#ifndef MAC_ADDRESS_H
#define MAC_ADDRESS_H
/*+++++++++++++++++++++++++++++++++++++++++++++
+++++++++++++++++++++++++++++++++++++++++++++++
   This file is used to config all devices 
   that will be communicate with each other.
+++++++++++++++++++++++++++++++++++++++++++++++
+++++++++++++++++++++++++++++++++++++++++++++++*/


/*********************
 *      INCLUDES
 *********************/
#include <Config.h>
#include <Arduino.h>


/*+++++++++++++++++++++++++++++++++++++++++++++
          MAC ADDRESS FOR BROADCAST
+++++++++++++++++++++++++++++++++++++++++++++++*/
static uint8_t macBroadcast[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; //dont modify this.

/*+++++++++++++++++++++++++++++++++++++++++++++
       MAC ADDRESS FOR ESPCORE PERSONS
+++++++++++++++++++++++++++++++++++++++++++++++*/
uint8_t macSlaves[NUMBER_OF_PERSON][6] = {{0xD2, 0xD1, 0xDA, 0x9E, 0x32, 0x64}, {0x24, 0xD7, 0xEB, 0x39, 0x01, 0xA0}};
char addressPeers[NUMBER_OF_PERSON][18] = {"d2:d1:da:9e:32:64", "24:d7:eb:39:01:a0"};


#endif