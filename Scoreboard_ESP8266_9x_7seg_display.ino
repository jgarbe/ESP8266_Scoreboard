/*
 * Developed by Jim Garbe & Chris Short
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* Create a WiFi access point and provide a web server on it. */

#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <Hash.h>


/*
 * Change this to 9 when using all 9 digits in the Scoreboard
 * 
 */
int numOfRegisters = 3; //  Just three digits
        //OR
//int numOfRegisters = 9;  // All Nine
/*
 * 
 * 
 */


const int dataPin = 2;              //Data Pin is connected to Pin D4
const int latchPin = 0;             //Latch Pin is connected to Pin D3 
const int clockPin = 4;             //Clock Pin is connected to Pin D2


int speed = 100;


int hs = 0;
int inn = 0;
int vs = 0;
int balls = 0;
int strikes = 0;
int outs = 0;



byte* registerState;
int allpins = ((numOfRegisters*8)-1); //array starting from 0
int numpins = numOfRegisters*8; //array starting from 0 

//Number Patterns (0-9)
//***Drains 0-7 must be connected to segments A-DP respectively when using prefabbed LED 7-segment displays.***
const byte numTable[] =
{
  B11111100,
  B01100000,
  B11011010,
  B11110010,
  B01100110,
  B10110110,
  B10111110,
  B11100000,
  B11111110,
  B11110110
}; 

// OR //
// binary from decimal 
//int segment[10] = {63,6,91,79,102,109,125,7,127,111 }; // for common cathode
//int segment[10] = {192,249,164,176,153,146,130,248,128,144 }; // for common anode



//---------------------------------------------------------------
//Our HTML webpage contents in program memory
const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head>
<script>
function confirmZero() {
    var a = document.createElement('a')
    var r = confirm("Are you sure you want to Zero everything");
    if (r == true) {
        window.location.reload(true);
    } 
   
}
</script>


<style>
body {
  background-color:blue;
  color:white;
  }
a {
  color:#88F2EC;
}
table,tr, td {text-align:center;
    border: 1px solid gray;}
th.numeh {
      border-color:white;
      border-style:solid solid none;
    }
td.numes {
      border-color:white;
      border-style:none solid none;
    } 
td.numea {
      border-style:none dashed solid solid;
    }
td.numeb {
      border-style:none dashed solid dashed;
    }
td.numec {
      border-style:none solid solid dashed;
    }
    
</style>


    <title>Church Hill Middle School Softball Scoreboard</title>

    </head>
<body>


<center>

<h1>Church Hill Middle School Softball Scoreboard</h1><br>

<table>
<tr>
<th colspan="3" class="numeh">
HOME
</th>
<th colspan="3" class="numeh">
INNINGS
</th>
<th colspan="3" class="numeh">
VISITOR 
</th>
</tr>
<tr>
<td colspan="3" class="numes">
<iframe name="homeScore" target="homeScore" src="homeScoreZero" height="100" width="100"></iframe>
</td>
<td colspan="3" class="numes">
<iframe name="innings" target="innings" src="inningsZero" height="100" width="100"></iframe>
</td>
<td colspan="3" class="numes">
<iframe name="visitorScore" target="visitorScore" src="visitorScoreZero" height="100" width="100"> </iframe>
</td>
</tr>
<tr>
<td class="numea">
<h3><a href="homeScoreMinus" target="homeScore">-</a></h3>
</td>
<td class="numeb">
<h3><a href="homeScoreZero" target="homeScore">0</a></h3>
</td>
<td class="numec">
<h3> <a href="homeScorePlus" target="homeScore" >+</a></h3>
</td>
<td class="numea">
<h3><a href="inningsMinus" target="innings">-</a></h3>
</td>
<td class="numeb">
<h3><a href="inningsZero" target="innings">0</a></h3>
</td>
<td class="numec">
<h3> <a href="inningsPlus" target="innings" >+</a></h3>
</td>
<td class="numea">
<h3><a href="visitorScoreMinus" target="visitorScore">-</a></h3>
</td>
<td class="numeb">
<h3><a href="visitorScoreZero" target="visitorScore">0</a></h3>
</td>
<td class="numec">
<h3> <a href="visitorScorePlus" target="visitorScore" >+</a></h3>
</td>
</tr>
<tr>
<th colspan="3" class="numeh">
BALLS
</th>
<th colspan="3" class="numeh">
STRIKES
</th>
<th colspan="3" class="numeh">
OUTS 
</th>
</tr>
<tr>
<td colspan="3" class="numes">
<iframe name="ballsN" target="ballsN" src="ballsZero" height="100" width="100"></iframe>
</td>
<td colspan="3" class="numes">
<iframe name="strikesN" target="strikesN" src="strikesZero" height="100" width="100"></iframe>
</td>
<td colspan="3" class="numes">
<iframe name="outsN" target="outsN" src="outsZero" height="100" width="100"> </iframe>
</td>
</tr>
<tr>
<td class="numea">
<h3><a href="ballsMinus" target="ballsN">-</a></h3>
</td>
<td class="numeb">
<h3><a href="ballsZero" target="ballsN">0</a></h3>
</td>
<td class="numec">
<h3> <a href="ballsPlus" target="ballsN" >+</a></h3>
</td>
<td class="numea">
<h3><a href="strikesMinus" target="strikesN">-</a></h3>
</td>
<td class="numeb">
<h3><a href="strikesZero" target="strikesN">0</a></h3>
</td>
<td class="numec">
<h3> <a href="strikesPlus" target="strikesN" >+</a></h3>
</td>
<td class="numea">
<h3><a href="outsMinus" target="outsN">-</a></h3>
</td>
<td class="numeb">
<h3><a href="outsZero" target="outsN">0</a></h3>
</td>
<td class="numec">
<h3> <a href="outsPlus" target="outsN" >+</a></h3>
</td>
</tr>
</table>

<table>

<tr>

<td>
<a href="ledOn" target="myIframe">TEST</a><br>
</td>
<td>
<a href="ledOff" target="myIframe">LEDs OFF</a>
</td>

<td>
<input type="button" value="Zero All" onclick="confirmZero()">
</td>
</tr></table>
<hr>
<iframe name="myIframe"  width="100" height="25" frameBorder="0">

<p>Garbonus & Chris Short</p>
</center>

</body>
</html>
)=====";
//---------------------------------------------------------------

/*The End of the HTML
 * 
 */




/* Set these to your desired credentials. */
/* NOTE:  Passord must be at least 8 characters long or default credentials will be used */
const char *ssid = "scoreboard";
const char *password = "wizsc0r3";
ESP8266WebServer server(80);

/* Just a little test message.  Go to http://192.168.4.1 in a web browser
 * connected to this access point to see it.
 */
void handleRoot() {
 Serial.println("You are on the main page");
 String s = MAIN_page; //Read HTML contents
 server.send(200, "text/html", s); //Send web page
}

void handleLEDon() { 
 Serial.println("LED on page");
 server.send(200, "text/html", "TEST"); //Send ADC value only to client ajax request

    
/* Test Pin values are in array form-- from 0 to (no. of pins-1).*/
for(int k=0;k < numpins;k++){
regWrite(k, HIGH);
delay(speed);
  Serial.println(k);
}
for(int k=allpins;k >= 0;k--){
  regWrite(k, LOW);
  delay(speed);
  Serial.println(k);
}
}

// Function Turn off all of the LEDS
void handleLEDoff() { 
 Serial.println("LED off page");
  for(int k=0;k < numpins;k++){
    regWrite(k, LOW);
     Serial.println(k); }
 server.send(200, "text/html", "OFF"); 
}

/*****************************
 * 
 * Home Score  Functions
 * 
 *****************************/

void handlehszero() { 
  String jig = "HomeScore";
  hs = tzero(jig,hs);
  handleUpdate();
}

 
void handlehsp() { 
  String jig = "HomeScore";
  hs = increase(jig,hs);
  handleUpdate();
}

void handlehsm() { 
  String jig = "HomeScore";
  hs = decrease(jig,hs);
  handleUpdate();
}

/*****************************
 * 
 * Innings Functions
 * 
 *****************************/
void handleinnzero() { 
  String jig = "Inning";
  inn = tzero(jig,inn);
  handleUpdate();
}

void handleinnp() { 
  String jig = "Inning";
  inn = increase(jig,inn);
  handleUpdate();
}

void handleinnm() { 
  String jig = "Inning";
  inn = decrease(jig,inn);
  handleUpdate();
}


/*****************************
 * 
 * Visitor Score Functions
 * 
 *****************************/
void handlevszero() { 
  String jig = "VisitorScore";
  vs = tzero(jig,vs);
  handleUpdate();
}
 
void handlevsp() { 
  String jig = "VisitorScore";
  vs = increase(jig,vs);
  handleUpdate();
}

void handlevsm() { 
  String jig = "VisitorScore";
  vs = decrease(jig,vs);
  handleUpdate();

}
/*****************************
 * 
 * Balls Functions
 * 
 *****************************/
void handleballszero() { 
  String jig = "Balls";
  balls = tzero(jig,balls);
  handleUpdate();
}
 
void handleballsp() { 
  String jig = "Balls";
  balls = increase(jig,balls);
  handleUpdate();
}

void handleballsm() { 
  String jig = "Balls";
  balls = decrease(jig,balls);
  handleUpdate();
}
/*****************************
 * 
 * Strikes Functions
 * 
 *****************************/
void handlestrikeszero() { 
  String jig = "Strikes";
  strikes = tzero(jig,strikes);
  handleUpdate();
}
 
void handlestrikesp() { 
  String jig = "Strikes";
  strikes = increase(jig,strikes);
  handleUpdate();
}

void handlestrikesm() { 
  String jig = "Strikes";
  strikes = decrease(jig,strikes);
  handleUpdate();
}
/*****************************
 * 
 * Outs Functions
 * 
 *****************************/
void handleoutszero() { 
  String jig = "Outs";
  outs = tzero(jig,outs);
  handleUpdate();
}
 
void handleoutsp() { 
  String jig = "Outs";
  outs = increase(jig,outs);
  handleUpdate();
}

void handleoutsm() { 
  String jig = "Outs";
  outs = decrease(jig,outs);
  handleUpdate();
}
/*****************************
 * 
 * 
 * 
 *****************************/

 
 // Where "nm" is the name and "g" is the count.
 
//Decrease the Count 

int decrease(String nm, int g) {
 Serial.println( nm + " -1");
     g<1?g=0:g = g-1;
  Serial.println("New"+ nm + " = " + g);
// Send the stuff
  String g1=String(g);
 server.send(200, "text/html",  "<body style=\"background-color:blue;color:white;text-align:center;\"><h1>" + g1 + "</h1></body>"); 
 return g;
  }
  
// Increase The Count

  int increase(String nm, int g) {
 Serial.println( nm + " +1");
 if (nm == "Balls"){   //Balls don't go over 3
     g>2?g=3:g+=1;
 } else if (nm == "Strikes" || nm == "Outs" ){  // Strikes and outs don't go over 2
  g>1?g=2:g+=1;
 } else {
  g +=1;
 }
// Send the stuff
  Serial.println("New"+ nm + " = " + g);
  String g1=String(g);
 server.send(200, "text/html", "<body style=\"background-color:blue;color:white;text-align:center;\"><h1>" + g1 + "</h1></body>"); 
 return g;
  }
  
 // Zero The Count
  
    int tzero(String nm, int g) {
 Serial.println( nm + " 0");
     g =0;
// Send the stuff
  Serial.println("New"+ nm + " = " + g);
  String g1=String(g);
 server.send(200, "text/html", "<body style=\"background-color:blue;color:white;text-align:center;\"><h1>" + g1 + "</h1></body>"); 
 return g;
  }
 /* /////////////////////////////////////////////////////////////////////////////////////
  *  handleUpdate()
  *  This function grabs all 9 digits and updates and sends them to the scoreboard.
  *  If the HOME, INNINGS, or VISITOR fields are used, it places a zero in the first digit when necessary.
  *  
  *////////////////////////////////////////////////////////////////////////////////////////
void handleUpdate() {                         // If a POST request is made to URI /UPDATE
  
//First, converting single digits to two for first three fields (HOME, INNINGS, VISITOR)

char hsString[2];
char innString[2];
char vsString[2];
// home Score
 sprintf(hsString, "%1d%1d", hs/10, hs%10);
  int hs1=hsString[0] - '0';
  int hs2=hsString[1] - '0';
  Serial.print("homeScore=");Serial.println(hsString);
  //Inning
  sprintf(innString, "%1d%1d", inn/10, inn%10);
  int inn1=innString[0] - '0';
  int inn2=innString[1] - '0';
  Serial.print("Inning=");Serial.println(innString);
    //Visitor Score
  sprintf(vsString, "%1d%1d", vs/10, vs%10);
  int vs1=vsString[0] - '0';
  int vs2=vsString[1] - '0';
  Serial.print("Visitor Score=");Serial.println(vsString);
  // Back to easy single digits
  //Balls
  Serial.print("Balls=");Serial.println(balls);
  //Strikes
  Serial.print("Strikes=");Serial.println(strikes);
  //Outs
  Serial.print("Outs=");Serial.println(outs);


/*
 * 
 * These are the 7 segment displays.  They are in order.
 * The first digit is sent to outs, then strikes, then balls,...
 * 
 * They are sent so quickly that it can't be seen during a change.
 * 
 */

// Uncomment lines to show other 7-seg displays
   
     digitalWrite(latchPin, 0);  // Unlock to send data
  //  shiftOut(dataPin, clockPin, numTable[outs]);  //This data sent first
  //  shiftOut(dataPin, clockPin, numTable[strikes]);
  //  shiftOut(dataPin, clockPin, numTable[balls]);
  //  shiftOut(dataPin, clockPin, numTable[vs2]);
   // shiftOut(dataPin, clockPin, numTable[vs1]);
   shiftOut(dataPin, clockPin, numTable[inn2]);  
 //   shiftOut(dataPin, clockPin, numTable[inn1]);
    shiftOut(dataPin, clockPin, numTable[hs2]);
    shiftOut(dataPin, clockPin, numTable[hs1]);  //This data send last
    digitalWrite(latchPin, 1);   // lock data in place
  } //End of "handleUpdate()"

/////////////////////////////////////////////////////////////////////////



// the heart of the program "shiftOut()" Function
void shiftOut(int myDataPin, int myClockPin, byte myDataOut) {
  // This shifts 8 bits out MSB first, 
  //on the rising edge of the clock,
  //clock idles low

  //internal function setup
  int i=0;
  int pinState;
  pinMode(myClockPin, OUTPUT);
  pinMode(myDataPin, OUTPUT);

  //clear everything out just in case to
  //prepare shift register for bit shifting
  digitalWrite(myDataPin, 0);
  digitalWrite(myClockPin, 0);

  //for each bit in the byte myDataOut�
  //NOTICE THAT WE ARE COUNTING DOWN in our for loop
  //This means that %00000001 or "1" will go through such
  //that it will be pin Q0 that lights. 
  for (i=7; i>=0; i--)  {
    digitalWrite(myClockPin, 0);

    //if the value passed to myDataOut and a bitmask result 
    // true then... so if we are at i=6 and our value is
    // %11010100 it would the code compares it to %01000000 
    // and proceeds to set pinState to 1.
    if ( myDataOut & (1<<i) ) {
      pinState= 1;
    }
    else {  
      pinState= 0;
    }

    //Sets the pin to HIGH or LOW depending on pinState
    digitalWrite(myDataPin, pinState);
    //register shifts bits on upstroke of clock pin  
    digitalWrite(myClockPin, 1);
    //zero the data pin after shift to prevent bleed through
    digitalWrite(myDataPin, 0);
  }

  //stop shifting
  digitalWrite(myClockPin, 0);
}
////////////////////////////////////////////////////////////////////////

void setup() {
	delay(1000);
	Serial.begin(115200);
	Serial.println();
	Serial.print("Configuring access point...");
	/* You can remove the password parameter if you want the AP to be open. */
	WiFi.softAP(ssid, password);

	IPAddress myIP = WiFi.softAPIP();
	Serial.print("AP IP address: ");
	Serial.println(myIP);

  server.on("/", handleRoot);     // The main page is called
  server.on("/ledOn", handleLEDon); // The LED testing
  server.on("/ledOff", handleLEDoff);  //Turn off all of the LEDS
  server.on("/homeScoreZero", handlehszero);  // Zero the Home Score
  server.on("/homeScoreMinus", handlehsm); // Subtract one from the Home Score
  server.on("/homeScorePlus", handlehsp); // Add to the Home Score
  server.on("/inningsZero", handleinnzero);  //Zero the Inning
  server.on("/inningsMinus", handleinnm); // Subtract from the Inning
  server.on("/inningsPlus", handleinnp); // Add to the Inning
   server.on("/visitorScoreZero", handlevszero);  //Zero the Visitor Score
   server.on("/visitorScoreMinus", handlevsm); //Subtract from the Visitor Score
  server.on("/visitorScorePlus", handlevsp);  //Add to the Visitor Score
     server.on("/ballsZero", handleballszero);  //Zero the Balls
   server.on("/ballsMinus", handleballsm); //Subtract from the Balls
  server.on("/ballsPlus", handleballsp);  //Add to the Balls
     server.on("/strikesZero", handlestrikeszero);  //Zero the Strikes
   server.on("/strikesMinus", handlestrikesm); //Subtract from the Strikes
  server.on("/strikesPlus", handlestrikesp);  //Add to the Strikes
   server.on("/outsZero", handleoutszero);  //Zero the  Outs
   server.on("/outsMinus", handleoutsm); //Subtract from the  Outs
  server.on("/outsPlus", handleoutsp);  //Add to the  Outs
	server.begin();
	Serial.println("HTTP server started");

  
  //set pins to output so you can control the shift register
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  

  //Initialize array
  registerState = new byte[numOfRegisters];
  for (size_t i = 0; i < numOfRegisters; i++) {
    registerState[i] = 0;
  }
}

void loop() {
	server.handleClient();



}

/****************************************************
 * 
 * Function to write directly to certain LEDs
 ****************************************************/
void regWrite(int pin, bool state){

  //Determines register
  int reg = pin / 8;
  //Determines pin for actual register
  int actualPin = pin - (8 * reg);

  //Begin session
  digitalWrite(latchPin, LOW);

  for (int i = 0; i < numOfRegisters; i++){

    //Get actual states for register
    byte* states = &registerState[i];

    //Update state
    if (i == reg){
      bitWrite(*states, actualPin, state);
    }
    //Write
    shiftOut(dataPin, clockPin, MSBFIRST, *states);
  }
  //End session
  digitalWrite(latchPin, HIGH);
}

