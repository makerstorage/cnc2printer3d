/*

Gnexlab extruder controller v4 - Dec 2015
WebSite: www.gnexlab.com
Developer: Nuri Erginer
Licence: GNU General Public License  see <http://www.gnu.org/licenses/>. 

This program is distributed in the hope that it will be useful, 
but WITHOUT ANY WARRANTY; without even the implied warranty of 
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
GNU General Public License for more details. 

*/

#include <Wire.h>
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"
#include <PID_v1.h>









#define DISPLAY_ADDRESS   0x70
Adafruit_7segment clockDisplay = Adafruit_7segment();

//Thermistor
#define T1 A1 //thermistor 1 connected to A1
#define Pot_Extruder_Pin  A2 //A3
//Outputs
#define ExtruderOutput 9 //10

#define s1 3
#define s2 4
#define s3 5

#define NUMTEMPS 20

#define RUNMODEFLAG 0
#define SETMODEFLAG 1

//#define TEMPERATURE_SAMPLES 10
#define TEMP_UPDATE_INTERVAL 200

#define buttonPin 2

double Current_Extruder_Temp = 0;
double Set_Extruder_Temp = 185;
int Extruder_Pot_Value;
boolean mode = 0; //default run mode

short temptable[NUMTEMPS][2];

// EPCOS 100K Thermistor (B57540G0104F000)
// Thermistor lookup table for RepRap Temperature Sensor Boards (http://make.rrrf.org/ts)
// Made with createTemperatureLookup.py (http://svn.reprap.org/trunk/reprap/firmware/Arduino/utilities/createTemperatureLookup.py)
// ./createTemperatureLookup.py --r0=100000 --t0=25 --r1=0 --r2=4700 --beta=4066 --max-adc=1023
// r0: 100000
// t0: 25
// r1: 0
// r2: 4700
// beta: 4066
// max adc: 1023

short temptable1[NUMTEMPS][2] = {
   {1, 841},
   {54, 255},
   {107, 209},
   {160, 184},
   {213, 166},
   {266, 153},
   {319, 142},
   {372, 132},
   {425, 124},
   {478, 116},
   {531, 108},
   {584, 101},
   {637, 93},
   {690, 86},
   {743, 78},
   {796, 70},
   {849, 61},
   {902, 50},
   {955, 34},
   {1008, 3}
};

// Honeywell 100K Thermistor (135-104LAG-J01)
// Made with createTemperatureLookup.py (http://svn.reprap.org/trunk/reprap/firmware/Arduino/utilities/createTemperatureLookup.py)
// ./createTemperatureLookup.py --r0=100000 --t0=25 --r1=0 --r2=4700 --beta=3974 --max-adc=1023
// r0: 100000
// t0: 25
// r1: 0
// r2: 4700
// beta: 3974
// max adc: 1023

short temptable2[NUMTEMPS][2] = {
{1, 916},
{54, 265},
{107, 216},
{160, 189},
{213, 171},
{266, 157},
{319, 146},
{372, 136},
{425, 127},
{478, 118},
{531, 110},
{584, 103},
{637, 95},
{690, 88},
{743, 80},
{796, 71},
{849, 62},
{902, 50},
{955, 34},
{1008, 2}
};

//RRRF 100K Thermistor
// Thermistor lookup table for RepRap Temperature Sensor Boards (http://make.rrrf.org/ts)
// Made with createTemperatureLookup.py (http://svn.reprap.org/trunk/reprap/firmware/Arduino/utilities/createTemperatureLookup.py)
// ./createTemperatureLookup.py --r0=100000 --t0=25 --r1=0 --r2=4700 --beta=3960 --max-adc=1023
// r0: 100000
// t0: 25
// r1: 0
// r2: 4700
// beta: 3960
// max adc: 1023

short temptable3[NUMTEMPS][2] = {
   {1, 929},
   {54, 266},
   {107, 217},
   {160, 190},
   {213, 172},
   {266, 158},
   {319, 146},
   {372, 136},
   {425, 127},
   {478, 119},
   {531, 111},
   {584, 103},
   {637, 96},
   {690, 88},
   {743, 80},
   {796, 71},
   {849, 62},
   {902, 50},
   {955, 34},
   {1008, 2}
};

long BUT_previousMillis = 0;        // will store last time LED was updated
long BUT_interval = 1000;           // interval at which to blink (milliseconds)

long Display_previousMillis =0;
long Display_interval = 1000;


unsigned long temp_prev_time;

/**********************************************************************************************
 * Arduino PID Library - Version 1
 * by Brett Beauregard <br3ttb@gmail.com> brettbeauregard.com
 **********************************************************************************************/

//PID
double OutputValueExtruder;
//double KP_Extruder = 22.2;
//double KI_Extruder = 1.08;
//double KD_Extruder = 114;


double KP_Extruder = 400;
double KI_Extruder = 0.08;
double KD_Extruder = 400;

/*
 #define  DEFAULT_Kp 22.2
  #define  DEFAULT_Ki 1.08
  #define  DEFAULT_Kd 114
*/

PID PIDExtruder(&Current_Extruder_Temp, &OutputValueExtruder, &Set_Extruder_Temp,KP_Extruder,KI_Extruder,KD_Extruder, DIRECT);
int WindowSizeExtruder = 5000;
unsigned long windowStartTimeExtruder;



void setup() {

pinMode(s1, INPUT);
pinMode(s2, INPUT);
pinMode(s3, INPUT);


setTermistorTable(digitalRead(s1),digitalRead(s2),digitalRead(s3));

//Serial.begin(9600);
  
  // put your setup code here, to run once:
clockDisplay.begin(DISPLAY_ADDRESS);

pinMode(ExtruderOutput, OUTPUT);
digitalWrite(ExtruderOutput, LOW);

initPID();
}

void sevenSegDisplay(){
   
   if (mode==RUNMODEFLAG){
    //clockDisplay.drawColon(false);
    clockDisplay.print((int)Current_Extruder_Temp, DEC);
    clockDisplay.writeDisplay();
 //   Serial.println("Current_Extruder_Temp");
//    Serial.println(Current_Extruder_Temp);
   }else {
    //clockDisplay.drawColon(true);
    clockDisplay.print((int)Set_Extruder_Temp, DEC);
    clockDisplay.writeDisplay();
  //  Serial.println("Set_Extruder_Temp");
//    Serial.println(Set_Extruder_Temp);
   
   }
}


void buttonPressed (){
digitalWrite(ExtruderOutput, LOW);
mode = SETMODEFLAG;
//Serial.println("Set Mode");
  
while(digitalRead(buttonPin)){
 

Extruder_Pot_Value = analogRead(Pot_Extruder_Pin); // read the value from the sensor
Extruder_Pot_Value = map(Extruder_Pot_Value, 0, 1023, 0, 350); 
    
Set_Extruder_Temp = Extruder_Pot_Value;


sevenSegDisplay();
delay(500);

  
} 

mode = RUNMODEFLAG;
//Serial.println("RUN Mode");

}//buttonPressed
  
  


void loop() {


  if (millis() - BUT_previousMillis > BUT_interval) 
  {
      BUT_previousMillis = millis();   // PID update rate
      if (digitalRead(buttonPin) == HIGH) {
        buttonPressed ();
      }  
  }//if millis
  
  
  if (millis() - Display_previousMillis > Display_interval) 
  {
      Display_previousMillis = millis();   // PID update rate
      sevenSegDisplay();
  
  }// if milis
  

  Manage_Temperatures ();




  /* 

  analogWrite(ExtruderOutput,255); 
  delay(500);
  analogWrite(ExtruderOutput,0); 
  delay(500); */
}//loop


//Manage Temperature

void read_Temperatures(){
  Current_Extruder_Temp = read_termistor(T1);
  
}


void Manage_Extruder_Temperature (){
  
  Extruder_Pot_Value = analogRead(Pot_Extruder_Pin); // read the value from the sensor
  Extruder_Pot_Value = map(Extruder_Pot_Value, 0, 1023, 0, 350); 
    
  Set_Extruder_Temp = Extruder_Pot_Value;
  
  read_Temperatures();
  
  PIDExtruder.Compute();
  analogWrite(ExtruderOutput,OutputValueExtruder); 
  
  if (Current_Extruder_Temp > Set_Extruder_Temp) {
  //  Current_Extruder_Temp = Set_Extruder_Temp;
    analogWrite(ExtruderOutput,0);
  } else {
    
  }  


  
}



void initPID(){
PIDExtruder.SetMode(AUTOMATIC);

}

void Manage_Temperatures (){
Manage_Extruder_Temperature ();

}

int read_termistor(int THERMISTOR_PIN)
{
  // int rawtemp = analogRead(THERMISTOR_PIN);
   int rawtemp =getSensorAverageValue (THERMISTOR_PIN,10,80);
   int current_celsius = 0;

   byte i;
   for (i=1; i<NUMTEMPS; i++)
   {
      if (temptable[i][0] > rawtemp)
      {
         int realtemp  = temptable[i-1][1] + (rawtemp - temptable[i-1][0]) * (temptable[i][1] - temptable[i-1][1]) / (temptable[i][0] - temptable[i-1][0]);

         if (realtemp > 255)
            realtemp = 255; 

         current_celsius = realtemp;

         break;
      }
   }

   // Overflow: We just clamp to 0 degrees celsius
   if (i == NUMTEMPS)
   current_celsius = 0;

   return current_celsius;
}



// get an average value from a jumpy or erratic input sensor.
const int getSensorAverageValue (const int sensorPin,
                                 const int numberOfSamples,
                                 const long timeGap)
{
  static int currentSample; // current sensor sample.
  static int currentValue;  // current sensor value.

  // current value works as a sum counter.
  currentValue = 0;

  // get sensor samples with delay and calculate the sum.
  for (int i = 0; i < numberOfSamples; i++) {
    // get sensor sample.
    currentSample = analogRead(sensorPin);

    // add sample to the sum counter.
    currentValue += currentSample;

    // delay some time for the next sample.
    delay(timeGap);
  }

  // get the average sensor value (ignore the fraction).
  return (currentValue / numberOfSamples);
}


void setTermistorTable(boolean v1,boolean v2,boolean v3){
//Serial.println(val);
 //clockDisplay.drawColon(false);
 if (!(v1)) {
     //temptable1
     memcpy(temptable, temptable1, sizeof(temptable1));
 }

 if (!(v2)) {
    //temptable2
    memcpy(temptable, temptable2, sizeof(temptable2));
 }

 if (!(v3)) {
    //temprtable 3
    memcpy(temptable, temptable3, sizeof(temptable3));
 } 
 
}  


