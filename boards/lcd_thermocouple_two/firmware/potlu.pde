



/*
 * Nuri Erginer
 * <http://3dprint.gnexlab.com/>
 * Extruder Controller Ver 3.2
 * Arduino üzerine gömülü yazılım
 * Bu yazılım 3d printerın extruder kısmını kontrol etmek için geliştirilmiştir.
 * Bu yazılım pc ile haberleşmez. usb üzerinden haberleşme mach3 cnc yazılımını etkilediği için bu versiyon geliştirildi
 * Isı kontrolü için pid yerine bang bang methodu kullanılmıştır. Daha büyük ısınma yüzeyi ile test edilecektir.
 */
#include <LiquidCrystal.h>
#include <PID_v1.h>



String firstLCDLine = String(16);
String secondLCDLine = String(16);
//LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);




#define Pot_Extruder_Pin   A3
#define Pot_Bed_Pin   A4

//Outputs
#define ExtruderOutput 8
#define BedOutput 9

long POT_previousMillis = 0;        // will store last time LED was updated
long POT_interval = 500;           // interval at which to blink (milliseconds)

long Display_previousMillis =0;
long Display_interval = 500;

#define TEMPERATURE_SAMPLES 10
#define TEMP_UPDATE_INTERVAL 200
unsigned long temp_prev_time;

double Current_Extruder_Temp = 0;
double Set_Extruder_Temp = 185;

double Current_Bed_Temp = 0;
double Set_Bed_Temp = 75;


int Extruder_Pot_Value;
int Bed_Pot_Value;





void setup() {
 
//  analogReference(EXTERNAL);
  
//  Serial.begin(4800);
//  Serial.println("Ready.");
  pinMode(ExtruderOutput, OUTPUT);
  digitalWrite(ExtruderOutput, LOW);

  pinMode(BedOutput, OUTPUT);
  digitalWrite(BedOutput, LOW);
  
  initPID();
  
  lcd.begin(16, 2);
  lcd.clear();
  lcd.print("gnexlab");  
}

void LCDdisplay(){
  
   firstLCDLine = "N:";
   firstLCDLine.concat ((int)Current_Extruder_Temp);
   firstLCDLine.concat (char(223));
   firstLCDLine.concat("C  ");
   
   firstLCDLine.concat("S:");
   firstLCDLine.concat ((int)Set_Extruder_Temp);
   firstLCDLine.concat (char(223));
   firstLCDLine.concat("C");

   int bosluk1 = 16-firstLCDLine.length();
   for (int i=0; i <bosluk1; i++){
   firstLCDLine.concat(" "); 
   } 
  
   lcd.setCursor (0, 0);
   lcd.print (firstLCDLine);

   secondLCDLine = "B:";
   secondLCDLine.concat((int)Current_Bed_Temp);
   secondLCDLine.concat (char(223));
   secondLCDLine.concat("C  ");
   
   secondLCDLine.concat("S:");
   secondLCDLine.concat((int)Set_Bed_Temp);
   secondLCDLine.concat (char(223));
   secondLCDLine.concat("C");
   

   int bosluk = 16-secondLCDLine.length();
   for (int i=0; i <bosluk; i++){
   secondLCDLine.concat(" "); 
   }
   
   lcd.setCursor (0, 1);
   lcd.print (secondLCDLine);
}

void loop() {

 if (millis() - POT_previousMillis > POT_interval) 
{
    POT_previousMillis = millis();   // PID update rate

Extruder_Pot_Value = analogRead(Pot_Extruder_Pin); // read the value from the sensor
Extruder_Pot_Value = map(Extruder_Pot_Value, 0, 1023, 0, 350); 

Set_Extruder_Temp = Extruder_Pot_Value;

Bed_Pot_Value = analogRead(Pot_Bed_Pin); // read the value from the sensor
Bed_Pot_Value = map(Bed_Pot_Value, 0, 1023, 0, 255); 

Set_Bed_Temp = Bed_Pot_Value;

//Serial.println(Bed_Pot_Value);
}//if millis


 if (millis() - Display_previousMillis > Display_interval) 
{
    Display_previousMillis = millis();   // PID update rate
    LCDdisplay();

   
Manage_Temperatures ();

}// if milis



//Serial.println(Motor_Pot_Value);

}//loop


 












