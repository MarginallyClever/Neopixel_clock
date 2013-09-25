//------------------------------------------------------------------------------
// Neopixel 60 light LED strip clock
// dan@marginallycelver.com 2011-06-21
//------------------------------------------------------------------------------
// Copyright at end of file.
// please see http://www.github.com/MarginallyClever/Neopixel_clock for more information.

//------------------------------------------------------------------------------
// constants
//------------------------------------------------------------------------------
// Serial communication bitrate
const long BAUD        = 57600;
// Maximum length of serial input message.
const int MAX_BUF      = 64;

const long ONE_DAY     = 24 * 60 * 60 * 1000;

//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------
#include <Adafruit_NeoPixel.h>


//------------------------------------------------------------------------------
// globals
//------------------------------------------------------------------------------
// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_RGB     Pixels are wired for RGB bitstream
//   NEO_GRB     Pixels are wired for GRB bitstream
//   NEO_KHZ400  400 KHz bitstream (e.g. FLORA pixels)
//   NEO_KHZ800  800 KHz bitstream (e.g. High Density LED strip)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, 6, NEO_GRB + NEO_KHZ800);

char buffer[MAX_BUF];
int sofar;

long start_seconds;
long start_time;

//------------------------------------------------------------------------------
// methods
//------------------------------------------------------------------------------


void setup() {
  // open communications
  Serial.begin(BAUD);

  // start the strip  
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  // start at a random time
  int h = rand() % 24;  // hour
  int m =rand() % 60;  // minute
  int s =rand() % 60;  // second
  start_seconds = h*60*60 + m*60 + s;
  start_time=millis();
}


void loop() {
  // get the time since the reference point
  long elapsed_seconds = ( millis() - start_time ) / 1000;
  long seconds_now = start_seconds + elapsed_seconds;
  // get the seconds, minutes, and hours.
  int s = seconds_now % 60;
  seconds_now /= 60;
  int m = seconds_now % 60;
  seconds_now /= 60;
  int h = seconds_now % 24;

  // make sure the elapsed_seconds doesn't overflow.
  if( elapsed_seconds > ONE_DAY ) {
    // more than a day?
    start_time -= ONE_DAY;
  }
  
  // display the clock
  int a,b,c;
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    a = ( i == s ? 255 : 0 );
    b = ( i == m ? 255 : 0 );
    c = ( floor((float)i/2.5) == h ? 255 : 0 );
    if( a==0 && b==0 && c==0 ) {
      if( (i%5)==0) a=b=c=(strip.numPixels()/24)*(i/5);
    }
    // this way we combine the colors when they overlap.
    strip.setPixelColor(i, strip.Color(a, b, c));
  }
  strip.show();
  delay(10);
}


/**
 * listen for instructions coming from the serial connection.
 * See: http://www.marginallyclever.com/2011/10/controlling-your-arduino-through-the-serial-monitor/
 */
void listenToSerial() {
  while(Serial.available() > 0) {
    buffer[sofar++]=Serial.read();
    if(buffer[sofar-1]==';') break;  // in case there are multiple instructions
  }
 
  // if we hit a semi-colon, assume end of instruction.
  if(sofar>0 && buffer[sofar-1]==';') {
    buffer[sofar]=0;
    
    // echo confirmation
    Serial.println(buffer);
 
    // do something with the command
    processCommand();
 
    // reset the buffer
    sofar=0;
  }
}


void processCommand() {
  char *ptr=buffer;
  int time[3];
  int n=0;
  while(ptr && ptr<buffer+sofar && n<3) {
    ptr=strchr(ptr,' ')+1;
    time[n]=atoi(ptr);
  }
  start_seconds = time[0]*60*60 + time[1]*60 + time[2];
  start_time = millis();
}

/**
* This file is part of Neopixel_clock.
*
* Neopixel_clock is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* Neopixel_clock is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with Neopixel_clock. If not, see <http://www.gnu.org/licenses/>.
*/
