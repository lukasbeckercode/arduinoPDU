# arduinoPDU
A simple communication protocol for communication between any device (e.g. PC) and an Arduino via Serial communication loosly based 
on the APDU protocol used in smart cards. 

## Basic command Syntax 
commands must be of even length! 
|Type |Size |Meaning |
|---|---|---| 
|CLA&INS |4 | The instruction to the Board |
|p1p2 |2 |any parameter, if the command needs any | 
|lc |2 | the length of the data that is sent with the command | 
|data |var |any data the board might need for processing the command | 

## Valid comands & examples
- 0100: Hello World
  - prints "hello World" 
  - can be used without p1p2 lc and data 
- 0101: echo
  - echos the data field back 
  - p1p2 must be set (value is ignored) 
  - example: 010100041234 -> answer is 12349000
- 0200: Store data 
  - stores data in the internal records array (type String)  
  - p1p2 tells the command where in this array to store the data, maximum value by default is 49   
  - example: 020001041234 -> Data is stored in records[1], returns 9000 upon success 
- 0201: get data 
  - returns the data stored in the requested record 
  - p1p2 tells the command which record to return 
  - lc and data are ignored 
  - example: 020101 -> returns record 1, if previous command was run "12349000"  
  
## Status words 
Status words can be compared to the HTTP Response Codes. <br /> 

|Status word |Meaning |
|---|---|
|9000 |Everything worked just fine! | 
|6700 |Wrong length: sent command has odd length| 
  
