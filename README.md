# arduinoPDU
A simple communication protocol for communication between any device (e.g. PC) and an Arduino via Serial communication loosly based 
on the APDU protocol used in smart cards. 
##Features 
### Storing and reading data 
The library provides a String array called records. You can store and read data here either using the according 
commands, or do it directly in your arduino sketch! 
###Better pin control
Internal checks keep track of the pinModes set to each pin and strictly enforces these pinModes. 
This means that a pin set to _INPUT_ cannot be set to _HIGH_ or _LOW_ by this library. 
The arduino allows the change of the pinMode and this library does as well, however the pin has to be specifically reset 
and set to a new pinMode to avoid mistakes in this matter
###Customisation 
Commands 9000 to 9999 are custom commands, so their handling can easily be implemented by anyone with a little experience 
in arduino development. Just take a quick look at the example.ino sketch. <br />
Although pointers are used to retrieve all the parts of the command, there is no need for a user of this library to 
understand pointers at all. Just put the "&" sings exactly where they are in the example.ino sketch 
and everything works. 
## Basic command Syntax 
<b>Commands must be of even length! </b>

Type |Size |Meaning |
---|---|---| 
CLA&INS |4 | The instruction to the Board |
p1p2 |2 |any parameter, if the command needs any | 
lc |2 | the length of the data that is sent with the command | 
data |var |any data the board might need for processing the command | 

## Valid commands & examples
###Class 01
- 0100: Hello World
  - prints "Hello World!9000" 
  - can be used without p1p2 lc and data 
- 0101: echo
  - echos the data field back 
  - p1p2 must be set (value is ignored) 
  - example: 010100041234 -> answer is 12349000
###Class 02
- 0200: Store data 
  - stores data in the internal records array (type String)  
  - p1p2 tells the command where in this array to store the data, maximum value by default is 49   
  - example: 020001041234 -> Data is stored in records[1], returns 9000 upon success 
- 0201: get data 
  - returns the data stored in the requested record 
  - p1p2 tells the command which record to return 
  - lc and data are ignored 
  - example: 020101 -> returns record 1, if previous command was run "12349000"  
###Class 03
- 0300: Set to _LOW_ 
  - sets the pin specified in p1p2 to _LOW_
  - requires the pinMode to be set to _OUTPUT_ (see below: 0311)
  - example: 030013 -> sets output pin 13 to _LOW_, returns "9000" upon success 
- 0301: Set to _HIGH_
  - sets the pin specified in p1p2 to _HIGH_
  - requires the pinMode to be set to _OUTPUT_ (see below: 0311)
  - example: 030113 -> set output pin 13 to _HIGH_ , returns "9000" upon success
- 0310: set pinMode to _INPUT_
  - sets the pinMode of p1p2 to _INPUT_, meaning that you can read the pin's value
  - necessary for 0302 command
  - example: 031013 -> pinMode of pin 13 is now _INPUT_, returns "9000" upon success 
- 0311: set pinMode to _OUTPUT_
  - sets the pinMode of p1p2 to _OUTPUT_, meaning that you can read the pin's value
  - necessary for e.g. 0300 and 0301 commands 
  - example: 031013 -> pinMode of pin 13 is now _OUTPUT_, returns "9000" upon success
- 0399: reset pinModes
  - sets all _OUTPUT_ pins to _LOW_ 
  - allows all pinModes to be reset

###Class 9X 
This command class can be personalized in the arduino sketch by the user. This comes in handy if you need 
functionality that isn't (yet) covered by this library. An example of how this can be used can be found
in the example.ino sketch. <br />
Since this command class is defined as 9X, you have CLAINS 9000 to 9999 available to your liking <br />
Example: 90010090 -> user defines, that 9001 means to turn a stepper motor as many steps as defined in 
the data field (90 in this example). The library itself doesn't return any Status word now. For semantic reasons, 
you should use SW9000 as an indicator of success, failure status word can be chosen freely. <br />
This apdu always must be of even length as well! 
## Status words 
Status words can be compared to the HTTP Response Codes. <br /> 

|Status word |Meaning |
|---|---|
|9000 |Everything worked just fine! | 
|6700 |Wrong length: sent command has odd length|
|6800 |Command unknown|
|6985 | pinMode already set |
|6986 | pin unavailable|

##Troubleshooting 
### I sent a command, and I get 6700 as a response, then nothing happens
This most likely means that the length of your command was not even. For parsing purposes, 
only commands with even lengths are permitted. <br /> 
Examples: 
- 01010003123 -> this is 11 chars long, so odd: returns 6700
- 010100041234 -> 12 chars long, so even: returns 12349000  

Also, the Arduino doesn't do anything if any status word other than 9000 is returned. <br />
Another possibility is, that the clains part has a length other than 4. So, for example 
- 01 returns 6700 
- 0100 returns "Hello World!9000"
### I sent a command, and I get 6800 as a response, then nothing happens
The command you sent is unknown to the library. If you meant to use an internal command, check for typos and look at Valid 
commands section of the Document. If you meant to use a custom command, make sure it starts with a "9". 
### I tried to set a pin to high, but I get 6986 as a response
This means your pinMode is either not set at all, or set to be an _INPUT_ <br />
Try the following(example uses pin 13, so replace all the 13's with your desired Pin): 
- Send the following command: 
  - 031113
  - if this returns "9000", continue. 
  - if this returns "6985", stop here and read the next section on 6985
- Test, if this fixed the issue: 
  - 030113
  - should return "9000"
  - pin 13 (or whichever pin you used) should be set to HIGH now
  
If this didn't work, please open an issue on this repo 
### I tried to set a pinMode using either 0310 or 0311, but I get 6985 as an answer  
This means the pin you are trying to assign a pinMode to already is used. You can either use a different or do the following: 
- check to which pinMode your desired pin is set currently
  - Send 030113(replace 13 with the desired pin again)
  - SW 9000 means it's an _OUTPUT_ , SW 6986 means it's an _INPUT_ 
  - if the current pinMode is the desired one, you're done here, everything works should work fine.
- if the current pinMode isn't the one you want: 
  - send 039013(replace 13 with desired pin) 
  - set the new pinMode
- if you want to reset all active pins 
  - send 0399
  - <b> CAUTION: THIS RESETS ALL THE PINS, THEY CANNOT BE USED WITHOUT REASSIGNING A PINMODE </b>