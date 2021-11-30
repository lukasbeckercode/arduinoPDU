#include "APDU.h"

/**
 * Global vars needed
 */
String capdu = ""; //whole capdu
String clains = ""; //cla and ins together
int p1p2 = 0; //p1p2 as one integer
int lc = 0; //length of data that is following
String data  = ""; // some data

/**
 * Bitmasks for in and output pins
 */
 int input_pins = 0;
 int output_pins = 0;
 int pwm_pins = 0;

/**
 * default constructor
 */
APDU::APDU() {

}

/**
 * Start a new Serial tx,
 */
void APDU::begin(){
    Serial.begin(9600);
    setPWMPins();
}

/**
 * Starts a new Serial TX
 * @param baud_rate baud rate used for Serial TX
 */
void APDU::begin(int baud_rate){
    Serial.begin(baud_rate);
    setPWMPins();
}

/**
 * Test method for debugging
 */
void APDU::writeMSG()
{
    Serial.write("Hello");
}

/**
 * parses the apdu, then runs the command behind it
 * @returns true, if the received command is a custom one, false if it is an internal apdu
 */
bool APDU::validateCAPDU()
{
    if(capdu.length() % 2 !=0 || capdu.length() < 4) //apdus must have even length
    {
        reset();
        Serial.write("6700");
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
    if (clains.charAt(0) == '9') //9000-9999 are custom commands, so return true
    {    //safety check on cla and ins pair
        if(clains.length() != 4)
        {
            reset();
            Serial.write("6700");
            return false;
        }
            return true;
    }
    runCommand();
    reset();
    return false;

}

/**
 * Runs a command based on the clains
 */
void APDU::runCommand()
{
    //safety check on cla and ins pair
    if(clains.length() != 4)
    {
        reset();
        Serial.write("6700");
    }


    if(clains =="0100") //prints hello world
    {
        Serial.write("Hello World!");
        Serial.write("9000");
    }
    else if ( clains =="0101") //echos the data
    {
        String print_data = data;
        int len = print_data.length();
        char pdArr[len+1];
        strcpy(pdArr,print_data.c_str());
        Serial.write(pdArr,len);
        Serial.write("9000");
    }
    else if (clains == "0102")
    {
        String print_data = data;
        int len = print_data.length();
        char pdArr[len+1];
        strcpy(pdArr,print_data.c_str());
        int delay_time = atoi(pdArr);
        Serial.write("9099");

        delay((int)delay_time);
        Serial.write("9000");
    }
    else if (clains == "0200") //stores the data in the record p1p2 (max 50)
    {
        records[p1p2] = data;
        Serial.write("9000");
    }
    else if (clains == "0201") //reads record from p1p2 (max 50)
    {
        String print_data = records[p1p2];
        int len = print_data.length();
        char pdArr[len+1];
        strcpy(pdArr,print_data.c_str());
        Serial.write(pdArr,len);
        Serial.write("9000");
    }
    else if (clains == "0300") //set pin p1p2 to LOW
    {

        if(checkPinStatus(false,p1p2))
        {
            digitalWrite(p1p2, 0);
            Serial.write("9000");
        }
        else
        {
            Serial.write("6986");
        }
    }
    else if (clains == "0301") //set p1p2 to HIGH
    {

        if(checkPinStatus(false,p1p2))
        {
            digitalWrite(p1p2, 1);
            Serial.write("9000");
        }
        else
        {
            Serial.write("6986"); //pin not available
        }
    }
    else if (clains == "0302") //digitalRead
    {
        if(checkPinStatus(true,p1p2))
        {
            Serial.write(digitalRead(p1p2));
            Serial.write("9000");
        }
        else
        {
            Serial.write("6986"); //pin not available
        }
    }
    else if (clains == "0303") //analogWrite
    {
        if(checkPinStatus(false,p1p2) && checkPWMPin(p1p2))
        {
            if(data.length()!=4)
            {
                Serial.write("6700");
            }
            else
            {
                char nums[4];
                strcpy(nums,data.c_str());
                int val = 0;
                for(int i = 0; i<4;i++)
                {
                    int add = ceil(((int)nums[i]-48)*pow(10,(3-i)));
                    val +=   add;
                }

                analogWrite(9,val);
                Serial.write("9000");
            }

        }
        else
        {
            Serial.write("6986");
        }
    }
    else if(clains == "0304")
    {
        if(checkPinStatus(true,p1p2))
        {
            if((p1p2)>13)
            {
                int input = analogRead(p1p2);
                if(input<1000){ //100-999
                    char inArr[3];
                    itoa(input,inArr,10);
                    Serial.write(inArr,3);
                } else if (input<100){ //10-99
                    char inArr[2];
                    itoa(input,inArr,10);
                    Serial.write(inArr,2);
                } else if (input<10) { //0-9
                    char inArr[1];
                    itoa(input,inArr,10);
                    Serial.write(inArr,1);
                }else{ // >999
                    char inArr[4];
                    itoa(input,inArr,10);
                    Serial.write(inArr,4);
                }
                Serial.write("9000");
            }
            else
            {
                Serial.write("6987"); //Not an analog pin
            }

        }
        else
        {
            Serial.write("6986");
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
            Serial.write("9000");
        }
        else
        {
            Serial.write("6985"); //pinMode already set
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
            Serial.write("9000");
        }
        else
        {
            Serial.write("6985");
        }
    }
    else if (clains=="0399") //turns off all pins, lets the pinMode be reset
    {
        resetPinStatus();
        Serial.write("9000");
    }
    else if (clains=="0390") //turns off p1p2 and lets it's pinMode be reset
    {
        resetPinStatus(p1p2);
        Serial.write("9000");
    }
    else
    {
        Serial.write("6800"); //unknown command
    }
}
/**
 * makes the arduino listen to serial tx's coming in and use them as apdus
 * makes sure it gets the whole capdu
 */
bool APDU::listen()
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
        return APDU::validateCAPDU(); //if a new capdu was sent, validate and parse it
    }
    return false;
}

/**
 * resets all public fields to not have data lingering around
 */
void APDU::reset()
{
    //reset all the fields
    capdu = "";
    clains = "";
    p1p2 = 0;
    lc = 0;
    data = "";
}

void APDU::getCAPDU(String *instruction, int *param, int *lenc, String *cdata )
{
   *instruction = clains;
   *param = p1p2;
   *lenc = lc;
   *cdata = data;

    reset();
}

/**
 * checks if a pin is already set to a pinMode
 * @param pin which pin to check
 * @return true, if the pin can be assigned to a new pinMode, false if it already has a designated pinMode
 */
bool APDU::checkPinAvailable(int pin)
{
    return (( bitRead(input_pins,pin) == 0) && (bitRead(output_pins,pin) == 0));
}

/**
 * checks if a pin is in the desired pinMode
 * @param in true if a pin is supposed to be an input, false if it is supposed to be an output pin
 * @param pin which pin should be checked
 * @return true, if the pin is in the expected pinMde according to the in param
 */
bool APDU::checkPinStatus(bool in, int pin)
{
    return in ? ( bitRead(input_pins,pin) == 1) : (bitRead(output_pins,pin) == 1);
}

/**
 * checks, which pin is an output, sets the output pins to LOW, then resets the pin status bitmask
 */
void APDU::resetPinStatus()
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

/**
 * allows a single pin to be reset
 * @param pin the pin to be reset
 */
void APDU::resetPinStatus(int pin)
{
    if(bitRead(output_pins,pin)==1)
    {
        bitWrite(output_pins,pin,0);
    }
    else if (bitRead(input_pins,pin) == 1)
    {
        digitalWrite(pin,0); //make sure the pin is turned off
        bitWrite(input_pins,pin,0);
    }
}

void APDU::setPWMPins()
{
    bitWrite(pwm_pins,3,1);
    bitWrite(pwm_pins,5,1);
    bitWrite(pwm_pins,6,1);
    bitWrite(pwm_pins,9,1);
    bitWrite(pwm_pins,10,1);
    bitWrite(pwm_pins,11,1);
}

bool APDU::checkPWMPin(int pin)
{
    return bitRead(pwm_pins,pin) == 1;
}
