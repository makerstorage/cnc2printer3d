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

#include <SPI.h>
#include "Adafruit_MAX31855.h"
/*
//#include  "MAX31855.h"

// ***** PIN DEFINITIONS *****
const  unsigned  char thermocoupleSO = 4;
const  unsigned  char thermocoupleCS = 3;
const  unsigned  char thermocoupleCLK = 5;

MAX31855  MAX31855(thermocoupleSO, thermocoupleCS, thermocoupleCLK);
*/

#define DO   4
#define CS   3
#define CLK  5
Adafruit_MAX31855 thermocouple(CLK, CS, DO);




#define DISPLAY_ADDRESS   0x71
Adafruit_7segment clockDisplay = Adafruit_7segment();

//Thermocouple


//pot
#define Pot_Extruder_Pin  A2 //A3
//Outputs
#define ExtruderOutput 9 //10



#define RUNMODEFLAG 0
#define SETMODEFLAG 1


#define TEMP_UPDATE_INTERVAL 200

#define buttonPin 2

double Current_Extruder_Temp = 0;
double Set_Extruder_Temp = 185;
int Extruder_Pot_Value;

boolean mode = 0; //default run mode



long BUT_previousMillis = 0;        
long BUT_interval = 1000;          

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

  clockDisplay.print(10, DEC);
  clockDisplay.writeDisplay();

  analogWrite(ExtruderOutput,255); 
  delay(500);
  analogWrite(ExtruderOutput,0); 
  delay(500); */
}//loop


//Manage Temperature

void read_Temperatures(){
//  Current_Extruder_Temp = read_termistor(T1);
  // Current_Extruder_Temp = MAX31855.readThermocouple(CELSIUS);
 thermocouple.readInternal();
 double c = thermocouple.readCelsius();
  
 if (isnan(c)) {
    
   } else {
     Current_Extruder_Temp = c;
   }

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

int read_termocouple()
{
  

  // return current_celsius;
}








