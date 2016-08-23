
/*

Gnexlab extruder controller v2.1 - August 2011
WebSite: store.gnexlab.com
Developer: Nuri Erginer
Licence: GNU General Public License  see <http://www.gnu.org/licenses/>. 

This program is distributed in the hope that it will be useful, 
but WITHOUT ANY WARRANTY; without even the implied warranty of 
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
GNU General Public License for more details. 

*/

#include <MAX6675.h>

int CS1 = 11;             // CS pin on MAX6675
int CS2 = 10;             // CS pin on MAX6675
int SO = 12;              // SO pin of MAX6675
int SCK = 13;             // SCK pin of MAX6675
int units = 1;            // Units to readout temp (0 = raw, 1 = ËšC, 2 = ËšF)
//float Current_Extruder_Temp = 0.0;  // Temperature output variable
//float Current_Bed_Temp = 0.0;

// Initialize the MAX6675 Library for our chip
MAX6675 temp1(CS1,SO,SCK,units);
MAX6675 temp2(CS2,SO,SCK,units);




/**********************************************************************************************
 * Arduino PID Library - Version 1
 * by Brett Beauregard <br3ttb@gmail.com> brettbeauregard.com
 **********************************************************************************************/
double OutputValueExtruder;
double OutputValueBed;

double KP_Extruder = 400;
double KI_Extruder = 0.08;
double KD_Extruder = 400;

double KP_Bed = 400;
double KI_Bed = 0.08;
double KD_Bed = 400;


PID PIDExtruder(&Current_Extruder_Temp, &OutputValueExtruder, &Set_Extruder_Temp,KP_Extruder,KI_Extruder,KD_Extruder, DIRECT);
PID PIDBed(&Current_Bed_Temp, &OutputValueBed, &Set_Bed_Temp,KP_Bed,KI_Bed,KD_Bed, DIRECT);









void read_Temperatures(){

      Current_Extruder_Temp = temp1.read_temp();
      Current_Bed_Temp = temp2.read_temp();
      
  // Current_Extruder_Temp = read_termistor(T1);
       // Current_Bed_Temp = read_termistor(T2);
     

}

void initPIDExtruder(){
 
//     windowStartTimeExtruder = millis();
     //tell the PID to range between 0 and the full window size
//     PIDExtruder.SetOutputLimits(0, WindowSizeExtruder);

     //turn the PID on
    PIDExtruder.SetMode(AUTOMATIC);
}

void initPIDBed(){
 
 //    windowStartTimeBed = millis();
     //tell the PID to range between 0 and the full window size
 //    PIDBed.SetOutputLimits(0, WindowSizeBed);

     //turn the PID on
    PIDBed.SetMode(AUTOMATIC);
}


void Manage_Extruder_Temperature (){
  read_Temperatures();
  PIDExtruder.Compute();
/*  
    
      unsigned long now = millis();
      if(now - windowStartTimeExtruder>WindowSizeExtruder)
      { //time to shift the Relay Window
       windowStartTimeExtruder += WindowSizeExtruder;
      }
      if(OutputValueExtruder > now - windowStartTimeExtruder) digitalWrite(ExtruderOutput,HIGH);
      else digitalWrite(ExtruderOutput,LOW); 
*/
   analogWrite(ExtruderOutput,OutputValueExtruder); 
 
  
}

void Manage_Bed_Temperature (){
  read_Temperatures();
  PIDBed.Compute();
  
     /*
      unsigned long now = millis();
      if(now - windowStartTimeBed>WindowSizeBed)
      { //time to shift the Relay Window
       windowStartTimeBed += WindowSizeBed;
      }
      if(OutputValueBed > now - windowStartTimeBed) digitalWrite(BedOutput,HIGH);
      else digitalWrite(BedOutput,LOW); 
 */ 
   analogWrite(BedOutput,OutputValueBed); 
  
  
}



void initPID(){
initPIDExtruder();
initPIDBed();
}

void Manage_Temperatures (){
Manage_Extruder_Temperature ();
Manage_Bed_Temperature ();
}


