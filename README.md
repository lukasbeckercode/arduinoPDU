# arduinoPDU
A simple communication protocol for communication between any device (e.g. PC) and an Arduino via Serial communication loosly based 
on the APDU protocol used in smart cards. 
## Features 
### Storing and reading data 
The library provides a String array called records. You can store and read data here either using the according 
commands, or do it directly in your arduino sketch! 
### Better pin control
Internal checks keep track of the pinModes set to each pin and strictly enforces these pinModes. 
This means that a pin set to _INPUT_ cannot be set to _HIGH_ or _LOW_ by this library. 
The arduino allows the change of the pinMode and this library does as well, however the pin has to be specifically reset 
and set to a new pinMode to avoid mistakes in this matter
### Customisation 
Commands 9000 to 9999 are custom commands, so their handling can easily be implemented by anyone with a little experience 
in arduino development. Just take a quick look at the example.ino sketch. <br />
Although pointers are used to retrieve all the parts of the command, there is no need for a user of this library to 
understand pointers at all. Just put the "&" signs exactly where they are in the example.ino sketch 
and everything works. 
## Documentation
An overview of all available commands and status words as well as an overview of the test-results can be found [here](https://lukasbeckercode.github.io/ArduinoPDU/)
