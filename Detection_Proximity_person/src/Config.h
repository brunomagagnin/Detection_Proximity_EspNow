#ifndef CONFIG_H 
#define CONFIG_H

/*+++++++++++++++++++++++++++++++++++++++++++++
+++++++++++++++++++++++++++++++++++++++++++++++
   This file is to config this code

+++++++++++++++++++++++++++++++++++++++++++++++
+++++++++++++++++++++++++++++++++++++++++++++++*/


/*+++++++++++++++++++++++++++++++++++++++++++++
    IF YOU WANT DEBUG FOR CODE WITH SERIAL
+++++++++++++++++++++++++++++++++++++++++++++++*/
#define DEBUG false
//---------------------------------------------

/*+++++++++++++++++++++++++++++++++++++++++++++
      IF YOU WANT TO USE EXTERN SENDERS
+++++++++++++++++++++++++++++++++++++++++++++++*/
#define EXTERN_SENDERS false
// true YES   false NO

/*+++++++++++++++++++++++++++++++++++++++++
Number of Peers that will be registered 
 ++++++++++++++++++++++++++++++++++++++++*/
#define NUMBER_OF_MACHINES 2
#define NUMBER_OF_EXTERN_PEERS 2
//-----------------------------------------

// PARAMETERS 
#define CHANNEL 13 //wifi channel

#define TARGET_ALERT -62 //sinal strength to trigge the alert

#define REFRESH 5000 
#define VIBRATION_TIME 1000
#define TIME_TO_STAY_CLOSE 20000 //not used

#endif