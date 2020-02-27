# ESP8266_Scoreboard
Using an ESP8266 and some shift registers to create a scoreboard controller.

The ESP8266 is large enough to serve a simple web page with AJAX that updates the scoreboard.
Some shift registers are in series and will update the score shown physically using 7-segment displays.  

A single shift register is needed for each 7-segment display.  
If larger 7-segment displays are used, the appropriate mosfets or relays are needed, but can still be controlled by the shift registers.  

The Nine Segments in the program.

Home Score   2 digits
Innings     2 digits
Visitor Score 2 digits
Balls 1 digit
Strikes 1 digit
Outs  1 digit

TTL = 9 digits
