# InfoBox
News, earthquake &amp; weather warnings (working on this section), location based weather. 

This appliction is currently working on an Arduino Uno with the following setup:

1. 1602 LCD on pins 7, 8, 9, 10, 11, 12 - Backlight on Analog 0
2. 2 LEDs on pin 5 & 6 respectively
3. DHT11 sensor for humidity and temperature readings
4. 2 Push buttons on pullup for changing screen state and message being displayed

The project works through serial connection at the moment, but will eventually be wireless. 
However, at the moment everything is done by serving the information to the Arduino through 
a python script.

Current limitations:

  1) Due to the Arduino serial buffer, only 64 characters can be served in one go. The code 
     on the Arduino side has two stages, in order to add more than 64 characters to the message
     board, but this has to be tweaked as it is not perfect at the moment. 64 char limitation 
     should be handled on both the python and Arduino side. 
     
  2) Currently there is a bit of an issue with the button interrups as it does not stop the loop
     when it is in the process of printing information. This can be fixed easily with a different
     logic, but takes time and wasn't prioritized. 
     
  3) There is no real local data unless no other data is available, hence the local temperature is 
     only there when the Arduino can't get a proper serial connection and doesn't receive data to
     print. This can be fixed by changing the boolean state to an integer and adding one step, however
     this was not prioritized in this version of the build. 
     
  4) In an attempt to get earthquake and weather warnings, there were very few options available that
     provided real-time warnings or 緊急地震速報 for free, I am currently working on getting a websocket
     service to allow a more automated update rather than putting stress on the python script.
     However, due to the unreliability of the code, I decided to remove it from this release.
