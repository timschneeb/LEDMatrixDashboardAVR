#ifndef SERIAL_H_
#define SERIAL_H_
class serial{
public:
    void recvWithStartEndMarkers();
    void showNewData();
    void ProcessRequest();
    byte numChars = 111;
    char receivedChars[111];
    boolean newData = false;
    struct serialrecv
    {
        String cmd;
        char param;
        char param2;
        char param3;
        bool extraIncluded;
        String extra;
    };
    typedef serialrecv Recv;
    Recv recv;
};
#endif