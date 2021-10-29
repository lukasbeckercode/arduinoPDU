#include "apdu.h"

/**
 * Global vars needed
 */
String capdu = ""; //whole capdu
String clains = ""; //cla and ins together
int p1p2 = 0; //p1p2 as one integer
int lc = 0; //length of data that is following
String data  = ""; // some data

/**
 * Constants
 */

int NUMBER_OF_PINS = 14;
/**
 * Bitmasks for in and output pins
 */
 int input_pins = 0;
 int output_pins = 0;
/**
 * default constructor
 */
apdu::apdu(){
}

/**
 * Start a new Serial tx,
 */
void apdu::begin(){
    Serial.begin(9600);
}

/**
 * Test method for debugging
 */
void apdu::writeMSG()
{
    Serial.println("Hello");
}

/**
 * parses the apdu, then runs the command behind it
 */
bool apdu::validateCAPDU()
{
    if(capdu.length() % 2 !=0) //apdus must have even length
    {
        reset();
        Serial.println("6700");
        return false;
    }

    char capdu_arr[capdu.length()]; //use this array to split the capdu into its parts
    capdu.toCharArray(capdu_arr,capdu.length()+1);

    String temp = "";
    int pos = 0; //keep track of which parts of the capdu have already been read

    for(int i = 0; i<4;i++) //cla and ins are the first 4 bytes
    {
        temp += capdu_arr[i];
        pos = i ;
    }

    clains = temp;

    temp = "";

    //read p1p2
    int p1p2_temp = 0 ;
    p1p2_temp += (((int)capdu_arr[++pos])-48)*10;
    p1p2_temp += ((int)capdu_arr[++pos])-48;
    p1p2 = p1p2_temp;

    //read lc
    int lc_temp = 0 ;
    lc_temp += (((int)capdu_arr[++pos])-48)*10;
    lc_temp += ((int)capdu_arr[++pos])-48;

    lc = lc_temp ;

    //read data based on lc
    for(int i = ++pos; i<pos+lc_temp; i++)
    {
        temp += capdu_arr[i];
    }

    data = temp;

    Serial.flush();
    /**____________________Command running___________________**/
    if (clains.charAt(0) == '9')
    {
        return true;
    }
    runCommand();
    reset();
    return false;

}

void apdu::runCommand()
{
    //safety check on cla and ins pair
    if(clains.length() != 4)
    {
        reset();
        Serial.println("6700");
    }


    if(clains =="0100") //prints hello world
    {
        Serial.print("Hello World!");
        Serial.println("9000");
    }
    else if ( clains =="0101") //echos the data
    {
        Serial.print(data);
        Serial.println("9000");
    }
    else if (clains == "0200") //stores the data in the record p1p2 (max 50)
    {
        records[p1p2] = data;
        Serial.println("9000");
    }
    else if (clains == "0201") //reads record from p1p2 (max 50)
    {
        String print_data = records[p1p2];
        Serial.print(print_data);
        Serial.println("9000");
    }
    else if (clains == "0300") //set pin p1p2 to LOW
    {

        if(checkPinStatus(false,p1p2))
        {
            digitalWrite(p1p2, 0);
            Serial.println("9000");
        }
        else
        {
            Serial.println("6986");
        }
    }
    else if (clains == "0301") //set p1p2 to HIGH
    {

        if(checkPinStatus(false,p1p2))
        {
            digitalWrite(p1p2, 1);
            Serial.println("9000");
        }
        else
        {
            Serial.println("6986");
        }
    }
    else if (clains =="0310") //set p1p2 to input
    {
        int pin = 0;
        bitWrite(pin,p1p2,1);
        if(checkPinAvailable(p1p2))
        {
            pinMode(p1p2,INPUT);
            input_pins = (input_pins | pin);
            Serial.println("9000");
        }
        else
        {
            Serial.println("6985");
        }
    }
    else if (clains == "0311") //set p1p2 to output
    {
        int pin = 0;
        bitWrite(pin,p1p2,1);
        if(checkPinAvailable(p1p2))
        {
            pinMode(p1p2,OUTPUT);
            output_pins = (output_pins | pin);
            Serial.println("9000");
        }
        else
        {
            Serial.println("6985");
        }
    }
    else if (clains=="0399") //turns off all pins, resets the pinMode of all of them
    {
        resetPinStatus();
    }
    else
    {
        Serial.println("6800");
    }
}
/**
 * makes the arduino listen to serial tx's coming in and use them as apdus
 * makes sure it gets the whole capdu
 */
bool apdu::listen()
{
    int serAV  = Serial.available(); //see how much data is incoming
    if(serAV != 0) //if there is data coming
    {
        delay(500); //wait for all the data to be in the queue
    }
    serAV = Serial.available(); //look at how much data is here now
    while(serAV>0) //as long as there still is data left
    {
        String temp = String(Serial.read()-48); //read incoming data, correct for ascii shift error
        temp.trim(); //remove newlines
        capdu += temp; //add data to capdu
        delay(10); //wait a little
        serAV = Serial.available(); //check if there still is incoming data
    }

    if(capdu.length()!=0)
    {
        return apdu::validateCAPDU(); //if a new capdu was sent, validate and parse it
    }
    return false;
}

void apdu::reset()
{
    //reset all the fields
    capdu = "";
    clains = "";
    p1p2 = 0;
    lc = 0;
    data = "";
}

void apdu::getCAPDU(String *instruction, int *param, int *lenc, String *cdata )
{
   *instruction = clains;
   *param = p1p2;
   *lenc = lc;
   *cdata = data;

    reset();
}

bool apdu::checkPinAvailable(int pin)
{
    return (( bitRead(input_pins,pin) == 0) && (bitRead(output_pins,pin) == 0));
}

bool apdu::checkPinStatus(bool in, int pin)
{

    return in ? ( bitRead(input_pins,pin) == 1) : (bitRead(output_pins,pin) == 1);
}

void apdu::resetPinStatus()
{
    for (int i = 0; i<NUMBER_OF_PINS;i++)
    {
        if(bitRead(output_pins,i)==1)
        {
            digitalWrite(i,0);
        }
    }
    input_pins = 0;
    output_pins = 0;
}
