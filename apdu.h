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
    void validateCAPDU();
    void writeMSG();
    void listen();
    String records[50];
};
#endif //APDU_LIBRARY_H
