#include <SD.h>
File sdcard_file;
int CS_pin = 4;     
#include <Wire.h>// Library for I2C  bus communication (with the RTC module)
#include <SPI.h> // Library for SPI  bus communication (with the SD card reader)
#include <TimeLib.h>
#include <DS1307RTC.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"
#define I2C_ADDRESS 0x3C
#define RST_PIN -1
SSD1306AsciiWire oled;

//#define signal_PIN 2 //Data pin connected to the input signal
#define filename "data.txt"
#define date_filename "setdate.txt"
#define minimum_time 200 
uint8_t  state, last_state;
unsigned long last_change_time = 0;

//==========================================================
String time2string() {
  String d;
  tmElements_t tm;
  if (RTC.read(tm)) {
    d=String(tm.Hour)+":"+String(tm.Minute);
  }
  return d;
}
//==========================================================
void Println(const String s) {
  Serial.println(s);
  Serial.println("String1");
  oled.println(s);
}
void Print(const String s) {
  Serial.print(s);
  oled.print(s);
}
void Println(const int s) {
  Serial.println(s);
  oled.println(s);
}
void Print(const int s) {
  Serial.print(s);
  oled.print(s);
}
void switch_detected() {
  String m  = (state == 1) ? ("ON,  ") : ("OFF, ");
  // PrintDate();
  m = m + time2string();// + ", " + String(Clock.getTemperature(), 1);
  Print(m);
//  Print2file(m);
}

//========================================================
byte h=0,v=0;    //variables used in for loops
const unsigned long period=50;  //little period used to prevent error
unsigned long kdelay=0;        // variable used in non-blocking delay 
const byte rows=4;             //number of rows of keypad
const byte columns=4;            //number of columnss of keypad
const byte Output[rows]={3,2,5,6}; //array of pins used as output for rows of keypad
const byte Input[columns]={7,8,9,10}; //array of pins used as input for columnss of keypad
byte keypad() // function used to detect which button is used 
{
 static bool no_press_flag=0;  //static flag used to ensure no button is pressed 
  for(byte x=0;x<columns;x++)   // for loop used to read all inputs of keypad to ensure no button is pressed 
  {
     if (digitalRead(Input[x])==HIGH);   //read evry input if high continue else break;
     else
     break;
     if(x==(columns-1))   //if no button is pressed 
     {
      no_press_flag=1;
      h=0;
      v=0;
     }
  }
  if(no_press_flag==1) //if no button is pressed 
  {
    for(byte r=0;r<rows;r++) //for loop used to make all output as low
    digitalWrite(Output[r],LOW);
    for(h=0;h<columns;h++)  // for loop to check if one of inputs is low
    {
      if(digitalRead(Input[h])==HIGH) //if specific input is remain high (no press on it) continue
      continue;
      else    //if one of inputs is low
      {
          for (v=0;v<rows;v++)   //for loop used to specify the number of row
          {
          digitalWrite(Output[v],HIGH);   //make specified output as HIGH
          if(digitalRead(Input[h])==HIGH)  //if the input that selected from first sor loop is change to high
          {
            no_press_flag=0;                //reset the no press flag;
            for(byte w=0;w<rows;w++) // make all outputs as low
            digitalWrite(Output[w],LOW);
            return v*4+h;  //return number of button 
          }
          }
      }
    }
  }
 return 50;
}

//==========================================================
void setup() {
  pinMode(CS_pin, OUTPUT);
  for(byte i=0;i<rows;i++)  //for loop used to make pin mode of outputs as output
  {
  pinMode(Output[i],OUTPUT);
  }
  for(byte s=0;s<columns;s++)  //for loop used to makk pin mode of inputs as inputpullup
  {
    pinMode(Input[s],INPUT_PULLUP);
  }
  Serial.begin(9600);
  if (SD.begin())
     {
     Serial.println("SD card is initialized and it is ready to use");
     } else
     {
     Serial.println("SD card is not initialized");
     return;
     }
     sdcard_file = SD.open("data.txt", FILE_WRITE); //Looking for the data.txt in SD card
     
     if (sdcard_file) { //If the file is found
     Serial.println("Writing to file is under process");
     sdcard_file.println("RESTART"); //Writing to file
     sdcard_file.close(); //Closing the file
     Serial.println("Done");
     }
     else {
     Serial.println("Failed to open the file");
     }
  //--------------------------------------------------------
  Wire.begin(); // Initialize the I2C
  Wire.setClock(400000L); // I2C speed
#if RST_PIN >= 0
  oled.begin(&Adafruit128x64, I2C_ADDRESS, RST_PIN);
#else // RST_PIN >= 0
  oled.begin(&Adafruit128x64, I2C_ADDRESS);
#endif // RST_PIN >= 0

  oled.setFont(System5x7);
  oled.clear();
#if INCLUDE_SCROLLING == 0
#error INCLUDE_SCROLLING must be non-zero.  Edit SSD1306Ascii.h
#endif //  INCLUDE_SCROLLING
  // Set auto scrolling at end of window.
  oled.setScrollMode(SCROLL_MODE_AUTO);
  Println(F("================="));
  Println(F("== TimeAndTide =="));
  Println(F("=== Wait4None ==="));
  Println(F("================="));
  //--------------------------------------------------------
//  Println(F("NothingComesWithEase"));
 
}
//==========================================================
unsigned long last_print = 0;
boolean stMa=0;
byte strH=0,stpH=0,strM=0,stpM=0,GtH=0,GtM=0,Th=0,Tm=0;  
void sub(String subj,boolean b){
          tmElements_t tm;
          RTC.read(tm);
          Serial.print(String(subj)+" from tm time:"+String(tm.Hour)+":"+String(tm.Minute));
          sdcard_file = SD.open("data.txt", FILE_WRITE);
          Serial.println(sdcard_file);
          if (sdcard_file) { //If the file is found
            if(stMa==0){
              oled.clear();
              oled.println(String(subj)+" is  Running @\n"+String(time2string()));
           strH=tm.Hour;
           strM=tm.Minute;
           Serial.println(time2string());
           sdcard_file.println("===========================");
           sdcard_file.println(String(subj)+" started---> "+String(time2string()));
           sdcard_file.close(); //Closing the file
           stMa=1;
        }else if(stMa==1){
          //oled.clear();
                    
//          //oled.println("Grand:"+String(GtH)+"h"+String(GtM)+"m");
//          oled.println("Learn And Then Earn");
          stpH=tm.Hour;
          stpM=tm.Minute;
           Serial.println(time2string());
           sdcard_file.print(String(subj)+" stopped---> ");
           sdcard_file.println(time2string()); //Writing to file
           sdcard_file.print("Total---> ");
           if((stpM-strM)<0)//||(stpM-strM)==0)
           {  if(b){
              Serial.println("Boolean is active1");
              GtH=GtH+stpH-strH-1;
              GtM=GtM+60-stpM-strM;
                }
            Th=stpH-strH-1;
            Tm=60-stpM-strM;    
            sdcard_file.print(stpH-strH-1);
            sdcard_file.print(":");
            sdcard_file.println(60-stpM-strM);
//            oled.println("Total:0h1m");
            }else if((stpM-strM)>0){ if(b){
                  GtH=GtH+stpH-strH;
                  GtM=GtM+stpM-strM;
                  }
             Th=stpH-strH;
             Tm=stpM-strM;
             sdcard_file.print(stpH-strH);
             sdcard_file.print(":");
             sdcard_file.println(stpM-strM); 
//             oled.println("Total:0h2m");
            }
           sdcard_file.println("===========================");
           sdcard_file.close();
           stMa=0;
           
           oled.print(String(subj)+ " Stopped @\n"+String(time2string())+"\n"+"T->"+String(Th)+":"+String(Tm)+"  |  G->"+String(GtH)+"h"+String(GtM)+"m");
           //oled.println();
        }
       }
  }
byte slpStH=0,slpStM=0;
void awake(){
  oled.clear();
  oled.println("WorkHardAndSomeDuas");
          tmElements_t tm;
          RTC.read(tm);
      sdcard_file = SD.open("data.txt", FILE_WRITE);
      Serial.println(sdcard_file);
       if (sdcard_file) { //If the file is found
       Serial.println(time2string());
       sdcard_file.print("++++++++++||Date:");//01/03/2020||++++++++++");
       sdcard_file.print(tm.Day);
       sdcard_file.print("/");
       sdcard_file.print(tm.Month);
       sdcard_file.print("/");
       sdcard_file.print(tmYearToCalendar(tm.Year));
       sdcard_file.println("||++++++++++");
       sdcard_file.print("**********||Awake@");
       sdcard_file.print(time2string());
       sdcard_file.println("||**********");
       sdcard_file.print("Slept:");//8h20m");
       sdcard_file.print(tm.Hour-slpStH);
       sdcard_file.print("h");
       sdcard_file.print(tm.Minute-slpStM);
       sdcard_file.println("m");
       sdcard_file.close();
       }
  }
  
void slp(){
  oled.clear();
  oled.println("MakeDua4rNight");
          tmElements_t tm;
          RTC.read(tm);
          slpStH=tm.Hour;
          slpStM=tm.Minute;
      sdcard_file = SD.open("data.txt", FILE_WRITE);
      Serial.println(sdcard_file);
       if (sdcard_file) { //If the file is found
       Serial.println(time2string());
       sdcard_file.println("GRAND TOTAL:"+ String(GtH)+"h"+ String(GtM)+"m");//10h20m");
       sdcard_file.print("Sleep@");
       sdcard_file.print(tm.Hour);
       sdcard_file.print(":");
       sdcard_file.println(tm.Minute);
       sdcard_file.println("%%%%%%%%%%%%%%%%%%%%%%%%%%%");
       sdcard_file.close();
       GtH=0;
       GtM=0;
       }
  }   
void loop() {
if(millis()-kdelay>period) //used to make non-blocking delay
  {
    kdelay=millis();  //capture time from millis function
switch (keypad())  //switch used to specify which button
   {
            case 0:
            Serial.println(1);
            sub("Salah",0);
       break;  
            case 1:
            Serial.println(2);
            sub("Exer",0);
       break;
            case 2:
            Serial.println(3);
            sub("Study",1);
       break;
            case 3:
            Serial.println("F1");
       break;
            case 4:
            Serial.println(4);
            //sub("EMT",1);
           
       break;
            case 5:
            Serial.println(5);
            //sub("ML",1);
       break;
            case 6:
            Serial.println(6);
            slp();
       break;
            case 7:
            Serial.println("F2");
       break;
            case 8:
            Serial.println(7);
       break;
            case 9:
            Serial.println(8);
       break;
            case 10:
            Serial.println(9);
       break;
            case 11:
            Serial.println("F3");
       break;
            case 12:
            Serial.println("Awaking");
            awake();
       break;
            case 13:
            Serial.println(0);
            sub("Break",0);
       break;
            case 14:
            Serial.println("Sleeping");
            slp();
       break;
            case 15:
            Serial.println("Enter");
       break;
            default:
            ;
    }
  }
  
 }
