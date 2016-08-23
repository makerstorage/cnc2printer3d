
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


int WindowSizeExtruder = 5000;
unsigned long windowStartTimeExtruder;

int WindowSizeBed = 5000;
unsigned long windowStartTimeBed;


// Thermistor lookup table for RepRap Temperature Sensor Boards (http://make.rrrf.org/ts)
// Made with createTemperatureLookup.py (http://svn.reprap.org/trunk/reprap/firmware/Arduino/utilities/createTemperatureLookup.py)
// ./createTemperatureLookup.py --r0=100000 --t0=25 --r1=0 --r2=4700 --beta=4066 --max-adc=1023
// r0: 100000
// t0: 25
// r1: 0
// r2: 4700
// beta: 4066
// max adc: 1023
#define NUMTEMPS 20
short temptable[NUMTEMPS][2] = {
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



void read_Temperatures(){

        Current_Extruder_Temp = read_termistor(T1);
        Current_Bed_Temp = read_termistor(T2);
     

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

int read_termistor(int THERMISTOR_PIN)
{
     int rawtemp = analogRead(THERMISTOR_PIN);
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
