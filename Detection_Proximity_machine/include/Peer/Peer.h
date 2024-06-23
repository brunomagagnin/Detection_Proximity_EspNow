#ifndef PEER_H
#define PEER_H
class Peer{

private:
    int rssi;
    int index;
    char macAddress[18];
    int rssiArray[4];

public:
    Peer();
    Peer(char *macAddress);

    char getMacAddress(int index);
    int getRssi();
    int getRssiArray(int index);
    
    void setRssi(int rssi);
    void setIndex(int index);
};
#endif