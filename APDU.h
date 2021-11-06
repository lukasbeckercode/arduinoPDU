#ifndef APDU_LIBRARY_H
#define APDU_LIBRARY_H
#if (ARDUINO >=100)
    #include "Arduino.h"
#else
    #include "WProgramm.h"
#endif

class APDU {
public:
    /**
     * CONSTANTS
     */
    int NUMBER_OF_PINS = 14;
    String records[50];
    /**
     * Constructor
     */
    APDU();

    /**
     * Methods
     */
    void begin();
    void begin(int);
    bool listen();
    void getCAPDU(String*, int*, int*, String*);

private:
    void runCommand();
    bool checkPinAvailable(int);
    bool checkPinStatus(bool,int);
    void resetPinStatus();
    void resetPinStatus(int);
    bool validateCAPDU();
    void writeMSG();
    void reset();
};
#endif //APDU_LIBRARY_H
