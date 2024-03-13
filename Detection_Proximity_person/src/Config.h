#ifndef CONFIG_H 
#define CONFIG_H


/*+++++++++++++++++++++++++++++++++++++++++++++
    IF YOU WANT DEBUG FOR CODE WITH SERIAL
+++++++++++++++++++++++++++++++++++++++++++++++*/
#define DEBUG true
//---------------------------------------------

/*+++++++++++++++++++++++++++++++++++++++++++++
      IF YOU WANT TO USE EXTERN SENDERS
+++++++++++++++++++++++++++++++++++++++++++++++*/
#define EXTERN_SENDERS true
// true YES   false NO

/*+++++++++++++++++++++++++++++++++++++++++
Number of Peers that will be registered 
 ++++++++++++++++++++++++++++++++++++++++*/
#define NUMBER_OF_MACHINES 2
#define NUMBER_OF_EXTERN_PEERS 2
//-----------------------------------------

// PARAMETERS 
#define CHANNEL 13 //wifi channel

#define TARGET_ALERT -62 //sinal strength
#define TARGET_DANGER -65 //sinal streagth

#define REFRESH 5000 
#define VIBRATION_TIME 1000

#endif