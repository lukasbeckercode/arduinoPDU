#ifndef APDU_LIBRARY_H
#define APDU_LIBRARY_H
#if (ARDUINO >=100)
    #include "Arduino.h"
#else
    #include "WProgramm.h"
#endif

class apdu {
public:
    apdu();
    void begin();
    bool validateCAPDU();
    void writeMSG();
    bool listen();
    void reset();
    void getCAPDU(String*, int*, int*, String*);
    void runCommand();
    bool checkPinAvailable(int);
    bool checkPinStatus(bool,int);
    void resetPinStatus();
    String records[50];
};
#endif //APDU_LIBRARY_H
