/*
 * Este programa es para desplegar la hora en 4 matrices MAZ7219
 * en dispositivos portatiles ESP-32.
 * 
 * Copyright 2021 Jose Luis Oliveros
 * 
 * This program is free software: you can redistribute it and/or 
 * modify it under the terms of the GNU General Public License as 
 * published by the Free Software Foundation, either version 3 of 
 * the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  
 * If not, see <http://www.gnu.org/licenses/>.
 * 
 */



#include <WiFi.h>
#include <time.h>
#include "LedControl.h"  // https://github.com/wayoda/LedControl

const char* ssid       = "WIFI-SSID";
const char* password   = "PASSWORD";

const char* ntpServer = "pool.ntp.org";
char* TimeZ = "COT+5"; // Time zone "COT+5" http://famschmid.net/timezones.html

const int time_delay = 500; 
const unsigned char scrollText[] PROGMEM = {"00:00"}; //Anuncio inicial

/*
 * VCC
 * GND
 * DIN D5 18
 * CS  D6 15
 * CLK D7 23
 * 
 */
int pinCLK = 18; // CLK to SCK  pin 13 and  (cf http://arduino.cc/en/Reference/SPI )
int pinCS  = 15; // CS to this pin, 10
int pinDIN = 23; // DIN to MOSI pin 11

const int numDevices = 4;      // number of MAX7219s used 
const long scrollDelay = 40;   // adjust scrolling speed
unsigned long bufferLong [14] = {0};  

// Create a LedControl for the first 4 devices...
LedControl lc=LedControl(pinDIN,pinCLK,pinCS,numDevices); //DATA | CLK | CS/LOAD | number of matrices

void setup() {
  Serial.begin(115200);
  Serial.printf("Connecting to %s ", ssid);

  //connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  Serial.println(" CONNECTED");
  
  int devices=lc.getDeviceCount();
  for(int address=0;address<devices;address++) {
    lc.shutdown(address,false);
    lc.setIntensity(address,0);    // From 0 to 15
    lc.clearDisplay(address);
  }
  scrollMessage(scrollText); //Anuncio inicial
  StartNTP(TimeZ);
  delay(3000);
}

void loop() {
  DispClock();
  delay(time_delay);
}


void StartNTP(char* Timezone){
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");  
  setenv("TZ", Timezone ,1);
}

char time_value[15];
void DispClock(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  time_t now = time(nullptr);
  String time = String(ctime(&now));
  time.trim();
  time.substring(11,19).toCharArray(time_value, 10);

//La variable time_value[0] es un Dec ASCII 

//Se envia a cada matriz el numero correspondiente
  setNum(3,time_value[0]); // H      2
  setNum(2,time_value[1]); // HH     8
  setNum(1,time_value[3]); // HH:M  20
  setNum(0,time_value[4]); // HH:MM 26

//Los dos puntos entre las horas y los minutos titilando
  if (time_value[7] % 2) {
    lc.setLed(2,2,7,false);
    lc.setLed(2,5,7,false);
  }
  else {
    lc.setLed(2,2,7,true);
    lc.setLed(2,5,7,true);
  }

//Indicador de segundos a ambos lados del dsplay.
int segs = timeinfo.tm_sec;

  if (segs>8) {
    lc.setLed(0,7,7,true);
//    lc.setLed(3,7,0,true);
  }
  if (segs>15) {
    lc.setLed(0,6,7,true);
//    lc.setLed(3,6,0,true);
  }
  if (segs>23) {
    lc.setLed(0,5,7,true);
//    lc.setLed(3,5,0,true);
  }
  if (segs>30) {
    lc.setLed(0,4,7,true);
 //   lc.setLed(3,4,0,true);
  }
  if (segs>38) {
    lc.setLed(0,3,7,true);
//    lc.setLed(3,3,0,true);
  }
  if (segs>45) {
    lc.setLed(0,2,7,true);
//    lc.setLed(3,2,0,true);
  }
  if (segs>53) {
    lc.setLed(0,1,7,true);
//    lc.setLed(3,1,0,true);
  }
  if (segs>60) {
    lc.setLed(0,0,7,true);
 //   lc.setLed(3,0,0,true);
  }
}

//Numeric Font Matrix (Arranged as 7x font data + 1x kerning data)
const unsigned char font5x7 [] PROGMEM = {
    B00000000,  //Space (Char 0x20)
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    3,//cambias el tamaño del espacio entre letras
 
    B01000000,  //!
    B01000000,
    B01000000,
    B01000000,
    B01000000,
    B00000000,
    B01000000,
    2,
 
    B10100000,  //"
    B10100000,
    B10100000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    4,
 
    B01010000,  //#
    B01010000,
    B11111000,
    B01010000,
    B11111000,
    B01010000,
    B01010000,
    6,
 
    B00100000,  //$
    B01111000,
    B10100000,
    B01110000,
    B00101000,
    B11110000,
    B00100000,
    6,
 
    B11000000,  //%
    B11001000,
    B00010000,
    B00100000,
    B01000000,
    B10011000,
    B00011000,
    6,
 
    B01100000,  //&
    B10010000,
    B10100000,
    B01000000,
    B10101000,
    B10010000,
    B01101000,
    6,
 
    B11000000,  //'
    B01000000,
    B10000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    3,
 
    B00100000,  //(
    B01000000,
    B10000000,
    B10000000,
    B10000000,
    B01000000,
    B00100000,
    4,
 
    B10000000,  //)
    B01000000,
    B00100000,
    B00100000,
    B00100000,
    B01000000,
    B10000000,
    4,
 
    B00000000,  //*
    B00100000,
    B10101000,
    B01110000,
    B10101000,
    B00100000,
    B00000000,
    6,
 
    B00000000,  //+
    B00100000,
    B00100000,
    B11111000,
    B00100000,
    B00100000,
    B00000000,
    6,
 
    B00000000,  //,
    B00000000,
    B00000000,
    B00000000,
    B11000000,
    B01000000,
    B10000000,
    3,
 
    B00000000,  //-
    B00000000,
    B11111000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    6,
 
    B00000000,  //.
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B11000000,
    B11000000,
    3,
 
    B00000000,  ///
    B00001000,
    B00010000,
    B00100000,
    B01000000,
    B10000000,
    B00000000,
    6,
 
    B01110000,  //0
    B10001000,
    B10011000,
    B10101000,
    B11001000,
    B10001000,
    B01110000,
    6,
 
    B01000000,  //1
    B11000000,
    B01000000,
    B01000000,
    B01000000,
    B01000000,
    B11100000,
    4,
 
    B01110000,  //2
    B10001000,
    B00001000,
    B00010000,
    B00100000,
    B01000000,
    B11111000,
    6,
 
    B11111000,  //3
    B00010000,
    B00100000,
    B00010000,
    B00001000,
    B10001000,
    B01110000,
    6,
 
    B00010000,  //4
    B00110000,
    B01010000,
    B10010000,
    B11111000,
    B00010000,
    B00010000,
    6,
 
    B11111000,  //5
    B10000000,
    B11110000,
    B00001000,
    B00001000,
    B10001000,
    B01110000,
    6,
 
    B00110000,  //6
    B01000000,
    B10000000,
    B11110000,
    B10001000,
    B10001000,
    B01110000,
    6,
 
    B11111000,  //7
    B10001000,
    B00001000,
    B00010000,
    B00100000,
    B00100000,
    B00100000,
    6,
 
    B01110000,  //8
    B10001000,
    B10001000,
    B01110000,
    B10001000,
    B10001000,
    B01110000,
    6,
 
    B01110000,  //9
    B10001000,
    B10001000,
    B01111000,
    B00001000,
    B00010000,
    B01100000,
    6,
 
    B00000000,  //:
    B11000000,
    B11000000,
    B00000000,
    B11000000,
    B11000000,
    B00000000,
    3,
 
    B00000000,  //;
    B11000000,
    B11000000,
    B00000000,
    B11000000,
    B01000000,
    B10000000,
    3,
 
    B00010000,  //<
    B00100000,
    B01000000,
    B10000000,
    B01000000,
    B00100000,
    B00010000,
    5,
 
    B00000000,  //=
    B00000000,
    B11111000,
    B00000000,
    B11111000,
    B00000000,
    B00000000,
    6,
 
    B10000000,  //>
    B01000000,
    B00100000,
    B00010000,
    B00100000,
    B01000000,
    B10000000,
    5,
 
    B01110000,  //?
    B10001000,
    B00001000,
    B00010000,
    B00100000,
    B00000000,
    B00100000,
    6,
 
    B01110000,  //@
    B10001000,
    B00001000,
    B01101000,
    B10101000,
    B10101000,
    B01110000,
    6,
 
    B01110000,  //A
    B10001000,
    B10001000,
    B10001000,
    B11111000,
    B10001000,
    B10001000,
    6,
 
    B11110000,  //B
    B10001000,
    B10001000,
    B11110000,
    B10001000,
    B10001000,
    B11110000,
    6,
 
    B01110000,  //C
    B10001000,
    B10000000,
    B10000000,
    B10000000,
    B10001000,
    B01110000,
    6,
 
    B11100000,  //D
    B10010000,
    B10001000,
    B10001000,
    B10001000,
    B10010000,
    B11100000,
    6,
 
    B11111000,  //E
    B10000000,
    B10000000,
    B11110000,
    B10000000,
    B10000000,
    B11111000,
    6,
 
    B11111000,  //F
    B10000000,
    B10000000,
    B11110000,
    B10000000,
    B10000000,
    B10000000,
    6,
 
    B01110000,  //G
    B10001000,
    B10000000,
    B10111000,
    B10001000,
    B10001000,
    B01111000,
    6,
 
    B10001000,  //H
    B10001000,
    B10001000,
    B11111000,
    B10001000,
    B10001000,
    B10001000,
    6,
 
    B11100000,  //I
    B01000000,
    B01000000,
    B01000000,
    B01000000,
    B01000000,
    B11100000,
    4,
 
    B00111000,  //J
    B00010000,
    B00010000,
    B00010000,
    B00010000,
    B10010000,
    B01100000,
    6,
 
    B10001000,  //K
    B10010000,
    B10100000,
    B11000000,
    B10100000,
    B10010000,
    B10001000,
    6,
 
    B10000000,  //L
    B10000000,
    B10000000,
    B10000000,
    B10000000,
    B10000000,
    B11111000,
    6,
 
    B10001000,  //M
    B11011000,
    B10101000,
    B10101000,
    B10001000,
    B10001000,
    B10001000,
    6,
 
    B10001000,  //N
    B10001000,
    B11001000,
    B10101000,
    B10011000,
    B10001000,
    B10001000,
    6,
 
    B01110000,  //O
    B10001000,
    B10001000,
    B10001000,
    B10001000,
    B10001000,
    B01110000,
    6,
 
    B11110000,  //P
    B10001000,
    B10001000,
    B11110000,
    B10000000,
    B10000000,
    B10000000,
    6,
 
    B01110000,  //Q
    B10001000,
    B10001000,
    B10001000,
    B10101000,
    B10010000,
    B01101000,
    6,
 
    B11110000,  //R
    B10001000,
    B10001000,
    B11110000,
    B10100000,
    B10010000,
    B10001000,
    6,
 
    B01111000,  //S
    B10000000,
    B10000000,
    B01110000,
    B00001000,
    B00001000,
    B11110000,
    6,
 
    B11111000,  //T
    B00100000,
    B00100000,
    B00100000,
    B00100000,
    B00100000,
    B00100000,
    6,
 
    B10001000,  //U
    B10001000,
    B10001000,
    B10001000,
    B10001000,
    B10001000,
    B01110000,
    6,
 
    B10001000,  //V
    B10001000,
    B10001000,
    B10001000,
    B10001000,
    B01010000,
    B00100000,
    6,
 
    B10001000,  //W
    B10001000,
    B10001000,
    B10101000,
    B10101000,
    B10101000,
    B01010000,
    6,
 
    B10001000,  //X
    B10001000,
    B01010000,
    B00100000,
    B01010000,
    B10001000,
    B10001000,
    6,
 
    B10001000,  //Y
    B10001000,
    B10001000,
    B01010000,
    B00100000,
    B00100000,
    B00100000,
    6,
 
    B11111000,  //Z
    B00001000,
    B00010000,
    B00100000,
    B01000000,
    B10000000,
    B11111000,
    6,
 
    B11100000,  //[
    B10000000,
    B10000000,
    B10000000,
    B10000000,
    B10000000,
    B11100000,
    4,
 
    B00000000,  //(Backward Slash)
    B10000000,
    B01000000,
    B00100000,
    B00010000,
    B00001000,
    B00000000,
    6,
 
    B11100000,  //]
    B00100000,
    B00100000,
    B00100000,
    B00100000,
    B00100000,
    B11100000,
    4,
 
    B00100000,  //^
    B01010000,
    B10001000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    6,
 
    B00000000,  //_
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B11111000,
    6,
 
    B10000000,  //`
    B01000000,
    B00100000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    4,
 
    B00000000,  //a
    B00000000,
    B01110000,
    B00001000,
    B01111000,
    B10001000,
    B01111000,
    6,
 
    B10000000,  //b
    B10000000,
    B10110000,
    B11001000,
    B10001000,
    B10001000,
    B11110000,
    6,
 
    B00000000,  //c
    B00000000,
    B01110000,
    B10001000,
    B10000000,
    B10001000,
    B01110000,
    6,
 
    B00001000,  //d
    B00001000,
    B01101000,
    B10011000,
    B10001000,
    B10001000,
    B01111000,
    6,
 
    B00000000,  //e
    B00000000,
    B01110000,
    B10001000,
    B11111000,
    B10000000,
    B01110000,
    6,
 
    B00110000,  //f
    B01001000,
    B01000000,
    B11100000,
    B01000000,
    B01000000,
    B01000000,
    6,
 
    B00000000,  //g
    B01111000,
    B10001000,
    B10001000,
    B01111000,
    B00001000,
    B01110000,
    6,
 
    B10000000,  //h
    B10000000,
    B10110000,
    B11001000,
    B10001000,
    B10001000,
    B10001000,
    6,
 
    B01000000,  //i
    B00000000,
    B11000000,
    B01000000,
    B01000000,
    B01000000,
    B11100000,
    4,
 
    B00010000,  //j
    B00000000,
    B00110000,
    B00010000,
    B00010000,
    B10010000,
    B01100000,
    5,
 
    B10000000,  //k
    B10000000,
    B10010000,
    B10100000,
    B11000000,
    B10100000,
    B10010000,
    5,
 
    B11000000,  //l
    B01000000,
    B01000000,
    B01000000,
    B01000000,
    B01000000,
    B11100000,
    4,
 
    B00000000,  //m
    B00000000,
    B11010000,
    B10101000,
    B10101000,
    B10001000,
    B10001000,
    6,
 
    B00000000,  //n
    B00000000,
    B10110000,
    B11001000,
    B10001000,
    B10001000,
    B10001000,
    6,
 
    B00000000,  //o
    B00000000,
    B01110000,
    B10001000,
    B10001000,
    B10001000,
    B01110000,
    6,
 
    B00000000,  //p
    B00000000,
    B11110000,
    B10001000,
    B11110000,
    B10000000,
    B10000000,
    6,
 
    B00000000,  //q
    B00000000,
    B01101000,
    B10011000,
    B01111000,
    B00001000,
    B00001000,
    6,
 
    B00000000,  //r
    B00000000,
    B10110000,
    B11001000,
    B10000000,
    B10000000,
    B10000000,
    6,
 
    B00000000,  //s
    B00000000,
    B01110000,
    B10000000,
    B01110000,
    B00001000,
    B11110000,
    6,
 
    B01000000,  //t
    B01000000,
    B11100000,
    B01000000,
    B01000000,
    B01001000,
    B00110000,
    6,
 
    B00000000,  //u
    B00000000,
    B10001000,
    B10001000,
    B10001000,
    B10011000,
    B01101000,
    6,
 
    B00000000,  //v
    B00000000,
    B10001000,
    B10001000,
    B10001000,
    B01010000,
    B00100000,
    6,
 
    B00000000,  //w
    B00000000,
    B10001000,
    B10101000,
    B10101000,
    B10101000,
    B01010000,
    6,
 
    B00000000,  //x
    B00000000,
    B10001000,
    B01010000,
    B00100000,
    B01010000,
    B10001000,
    6,
 
    B00000000,  //y
    B00000000,
    B10001000,
    B10001000,
    B01111000,
    B00001000,
    B01110000,
    6,
 
    B00000000,  //z
    B00000000,
    B11111000,
    B00010000,
    B00100000,
    B01000000,
    B11111000,
    6,
 
    B00100000,  //{
    B01000000,
    B01000000,
    B10000000,
    B01000000,
    B01000000,
    B00100000,
    4,
 
    B10000000,  //|
    B10000000,
    B10000000,
    B10000000,
    B10000000,
    B10000000,
    B10000000,
    2,
 
    B10000000,  //}
    B01000000,
    B01000000,
    B00100000,
    B01000000,
    B01000000,
    B10000000,
    4,
 
    B00000000,  //~
    B00000000,
    B00000000,
    B01101000,
    B10010000,
    B00000000,
    B00000000,
    6,
 
    B01100000,  // (Char 0x7F)
    B10010000,
    B10010000,
    B01100000,
    B00000000,
    B00000000,
    B00000000,
    5,
    
    B00000000,  // smiley
    B01100000,
    B01100110,
    B00000000,
    B10000001,
    B01100110,
    B00011000,
    5
};



// array que contiene todos los elementos del reloj
byte cero[8]= {
    B00111000, // 0
    B01001100,
    B01001100,
    B01010100,
    B01010100,
    B01100100,
    B01100100,
    B00111000
};

byte uno[8]= {
    B00010000,  // 1
    B00110000,
    B00010000,
    B00010000,
    B00010000,
    B00010000,
    B00010000,
    B00111000
};

byte dos[8]= {
    B00111000,  // 2
    B01000100,
    B00000100,
    B00000100,
    B00001000,
    B00010000,
    B00100000,
    B01111100
};

byte tres[8]= {
    B00111000,  // 3
    B01000100,
    B00000100,
    B00011000,
    B00000100,
    B00000100,
    B01000100,
    B00111000
 };
 
byte cuatro[8]= {
    B00001000,  // 4
    B00011000,
    B00101000,
    B01001000,
    B01001000,
    B01111100,
    B00001000,
    B00001000
};

byte cinco[8]= {
    B01111100,  // 5
    B01000000,
    B01000000,
    B01111000,
    B00000100,
    B00000100,
    B01000100,
    B00111000
};

byte seis[8]= {
    B00111000,  // 6
    B01000100,
    B01000000,
    B01111000,
    B01000100,
    B01000100,
    B01000100,
    B00111000
};

byte siete[8]= {
    B01111100,  // 7
    B00000100,
    B00000100,
    B00001000,
    B00010000,
    B00100000,
    B00100000,
    B00100000
};

byte ocho[8]= {
    B00111000,  // 8
    B01000100,
    B01000100,
    B00111000,
    B01000100,
    B01000100,
    B01000100,
    B00111000
};

byte nueve[8]= {
    B00111000,  // 9
    B01000100,
    B01000100,
    B01000100,
    B00111100,
    B00000100,
    B01000100,
    B00111000
};

void setNum(int dis, int num) {
if(num == 48){ //Dec ASCII 48 = 0 
    for (int i = 0; i < 8; i++) {    // bucle itera 8 veces por el array
    lc.setRow(dis,i,cero[i]);   // establece fila con valor de array cero en misma posicion
  }
}
if(num == 49){ //Dec ASCII 49 = 1
    for (int i = 0; i < 8; i++) {    // bucle itera 8 veces por el array
    lc.setRow(dis,i,uno[i]);   // establece fila con valor de array uno en misma posicion
  }
}
if(num == 50){ //Dec ASCII 50 = 2
    for (int i = 0; i < 8; i++) {    // bucle itera 8 veces por el array
    lc.setRow(dis,i,dos[i]);   // establece fila con valor de array dos en misma posicion
  }
}
if(num == 51){ //Dec ASCII 51 = 3
    for (int i = 0; i < 8; i++) {    // bucle itera 8 veces por el array
    lc.setRow(dis,i,tres[i]);   // establece fila con valor de array tres en misma posicion
  }
}
if(num == 52){ //Dec ASCII 52 = 4
    for (int i = 0; i < 8; i++) {    // bucle itera 8 veces por el array
    lc.setRow(dis,i,cuatro[i]);   // establece fila con valor de array cuatro en misma posicion
  }
}
if(num == 53){ //Dec ASCII 53 = 5
    for (int i = 0; i < 8; i++) {    // bucle itera 8 veces por el array
    lc.setRow(dis,i,cinco[i]);   // establece fila con valor de array cinco en misma posicion
  }
}
if(num == 54){ //Dec ASCII 54 = 6
    for (int i = 0; i < 8; i++) {    // bucle itera 8 veces por el array
    lc.setRow(dis,i,seis[i]);   // establece fila con valor de array seis en misma posicion
  }
}
if(num == 55){ //Dec ASCII 55 = 7
    for (int i = 0; i < 8; i++) {    // bucle itera 8 veces por el array
    lc.setRow(dis,i,siete[i]);   // establece fila con valor de array siete en misma posicion
  }
}
if(num == 56){ //Dec ASCII 56 = 8
    for (int i = 0; i < 8; i++) {    // bucle itera 8 veces por el array
    lc.setRow(dis,i,ocho[i]);   // establece fila con valor de array ocho en misma posicion
  }
}
if(num == 57){ //Dec ASCII 57 = 9
    for (int i = 0; i < 8; i++) {    // bucle itera 8 veces por el array
    lc.setRow(dis,i,nueve[i]);   // establece fila con valor de array nueve en misma posicion
  }
}
}

// Scroll Message
void scrollMessage(const unsigned char * messageString) {
    int counter = 0;
    int myChar=0;
    do {
        // read back a char 
        myChar =  pgm_read_byte_near(messageString + counter); 
        if (myChar != 0){
            loadBufferLong(myChar);
        }
        counter++;
    } 
    while (myChar != 0);
}

// Load character into scroll buffer
void loadBufferLong(int ascii){
    if (ascii >= 0x20 && ascii <=0x7f){
        for (int a=0;a<7;a++){                      // Loop 7 times for a 5x7 font
            unsigned long c = pgm_read_byte_near(font5x7 + ((ascii - 0x20) * 8) + a);     // Index into character table to get row data
            unsigned long x = bufferLong [a*2];     // Load current scroll buffer
            x = x | c;                              // OR the new character onto end of current
            bufferLong [a*2] = x;                   // Store in buffer
        }
        byte count = pgm_read_byte_near(font5x7 +((ascii - 0x20) * 8) + 7);     // Index into character table for kerning data
        for (byte x=0; x<count;x++){
            rotateBufferLong();
            printBufferLong();
            delay(scrollDelay);
        }
    }
}

// Rotate the buffer
void rotateBufferLong(){
    for (int a=0;a<7;a++){                      // Loop 7 times for a 5x7 font
        unsigned long x = bufferLong [a*2];     // Get low buffer entry
        byte b = bitRead(x,31);                 // Copy high order bit that gets lost in rotation
        x = x<<1;                               // Rotate left one bit
        bufferLong [a*2] = x;                   // Store new low buffer
        x = bufferLong [a*2+1];                 // Get high buffer entry
        x = x<<1;                               // Rotate left one bit
        bitWrite(x,0,b);                        // Store saved bit
        bufferLong [a*2+1] = x;                 // Store new high buffer
    }
}
  
// Display Buffer on LED matrix
void printBufferLong(){
  for (int a=0;a<7;a++){                    // Loop 7 times for a 5x7 font
    unsigned long x = bufferLong [a*2+1];   // Get high buffer entry
    byte y = x;                             // Mask off first character
    lc.setRow(3,a,y);                       // Send row to relevent MAX7219 chip
    x = bufferLong [a*2];                   // Get low buffer entry
    y = (x>>24);                            // Mask off second character
    lc.setRow(2,a,y);                       // Send row to relevent MAX7219 chip
    y = (x>>16);                            // Mask off third character
    lc.setRow(1,a,y);                       // Send row to relevent MAX7219 chip
    y = (x>>8);                             // Mask off forth character
    lc.setRow(0,a,y);                       // Send row to relevent MAX7219 chip
  }
}
