# ESP8266_Scoreboard
Using an ESP8266 and some shift registers to create a scoreboard controller.

The ESP8266 is large enough to serve a simple web page with AJAX that updates the scoreboard.
Some shift registers are in series and will update the score shown physically using 7-segment displays.  

A single shift register is needed for each 7-segment display.  
If larger 7-segment displays are used, the appropriate mosfets or relays are needed, but can still be controlled by the shift registers.  
