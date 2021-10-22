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
void apdu::validateCAPDU()
{
    if(capdu.length() % 2 !=0) //apdus must have even length
    {
        Serial.println("6700");
        return;
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
    //safety check on cla and ins pair
    if(clains.length() != 4)
    {
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

    //reset all the fields
    capdu = "";
    clains = "";
    p1p2 = 0;
    lc = 0;
    data = "";
}

/**
 * makes the arduino listen to serial tx's coming in and use them as apdus
 * makes sure it gets the whole capdu
 */
void apdu::listen()
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
        apdu::validateCAPDU(); //if a new capdu was sent, validate and parse it
    }
}
