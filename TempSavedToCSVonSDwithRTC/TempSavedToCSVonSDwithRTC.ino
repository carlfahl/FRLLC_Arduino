/*
This Program will
Read temperature and 
Save as .CSV file
To an SD card
In real time 
Using a REAL TIME CLOCK
*/

//Calls the SD, Wire, and RTC librarys
#include <SD.h>
#include <Wire.h>
#include "RTClib.h"

RTC_DS1307 RTC;

//MUST ALWAYS SET CS PIN
const int CS_PIN = 10;

//This is the pin the SD card
//Will draw power from
//So set it high
const int SD_POW_PIN = 8;

//Set the POWER Pin for RTC
const int RTC_POW_PIN = A3;

//Set the GROUND Pin for RTC
const int RTC_GND_PIN = A2;

//Initialize strings
String year, month, day, hour, minute, second, time, data;

//Temperature sensor on analog pin 0
int TEMP = 0;

void setup()
{
  Serial.begin(9600);
  Serial.println(F("Initializing Card"));
  
  //Set CS Pin as an output
  pinMode(CS_PIN, OUTPUT);
  
  //Set the power high
  pinMode(SD_POW_PIN, OUTPUT);
  digitalWrite(SD_POW_PIN, HIGH);
  
  //Set RTC Pin power/ground as output
  pinMode(RTC_POW_PIN, OUTPUT);
  pinMode(RTC_GND_PIN, OUTPUT);
  
  //Setup power and ground pins
  digitalWrite(RTC_POW_PIN, HIGH);
  digitalWrite(RTC_GND_PIN, LOW);
  
  //Initiate the I2C bus and the RTC library
  Wire.begin();
  RTC.begin();

  if (RTC.isrunning());
  {
    RTC.adjust(DateTime(__DATE__, __Time__));
  }

//Has the card been initialized?
  if (!SD.begin(CS_PIN))
  {
    Serial.println(F("Card Failure"));
    return;
  }
  
  Serial.println(F("Card Ready"));
  
  //Read the configuration information (speed.txt)
  File commandFile = SD.open("speed.txt");
  if (commandFile)
  {
    Serial.println(F("Reading Command File"));
    
    while(commandFile.available())
    {
      refresh_rate = commandFile.parseInt();
    }
    
    Serial.print(F("Refresh Rate = "));
    Serial.print(refresh_rate);
    Serial.println(F(ms));
    commandfile.close();
  }
  
  else
  {
    Serial.println(F("Could not read command file"));
    return;
  }
  
  //Display a header
  File dataFile = SD.open("log.csv", FILE_WRITE);
  if (dataFile)
  {
    dataFile.println(F("\nNew Log Started"));
    dataFile.print(F("Date,"));
    dataFile.print(F("Time,"));
    dataFile.print(F("Temperature (F),"));
    dataFile.print(F("Temperature (C),"));
    dataFile.println(F("Temperature (K)"));
    
    //Data isnt written until the connection is closed
    dataFile.colse();
  
    //Display a header on the Serial Monitor
    Serial.println(F("\nNew Log Started"));
    Serial.print(F("Time (ms)"));
    Serial.print(F("  "));
    Serial.print(F("Temperature (F)"));
    Serial.print(F("  "));
    Serial.print(F("Temperature (C)"));
    Serial.print(F("  "));
    Serial.println(F("Temperature (K)"));
  }
  
  else
  {
    Serial.println(F("Couldn't open log file"));
  }
}

void loop()
{
  //Set the current date and time and store them as strings
  DateTime datetime = RTC.now();
  
  year = String(datetime.year(), DEC);
  month = String(datetime.month(), DEC);
  day = String(datetime.day(), DEC);
  hour = String(datetime.hour(), DEC);
  minute = String(datetime.minute(), DEC);
  seconds = String(datetime.seconds(), DEC);
  
  //Condence to date and time
  date = year + "/" + month "/" + day
  time = hour + ":" + minute + ":" + seconds
  
  //Read the temperature sensor in bits
  int BIT = analogRead(TEMP);
  //Convert bits into farenheit
  float TempF =(BIT / 1024.0) * (5000 / 10);
  //Convert farenheit into celsius
  float TempC = ( (TempF - 32) * 5) / 9;
  //conver celsius into kelvin
  float TempK = 273 + TempC;
  
  //Open a file and write to it
  File dataFile = SD.open("log.csv", FILE_WRITE);
  
  if (dataFile)
  {
    //Displaying the data in log.csv
    dataFile.print(F(date));
    dataFile.print(F(","));
    dataFile.print(F(time));
    dataFile.print(F(","));
    dataFile.print(F(TempF));
    dataFile.print(F(","));
    dataFile.print(F(TempC));
    dataFile.print(F(","));
    dataFile.println(F(TempK));
    
    //Data isnt written to log.csv
    //until the program is closed
    dataFile.close();
    
    //Displaying the data in the Serial Monitor
    Serial.print(F(date));
    Serial.print(F("  "));
    Serial.print(F(time));
    Serial.print(F("  "));
    Serial.print(F(TempF));
    Serial.print(F("F  "));
    Serial.print(F(TempC));
    Serial.print(F("C  "));
    Serial.print(F(TempK));
    Serial.println(F("K"));
  }
  
  else
  {
    Serial.println(F("Couldn't open log.csv"));
  }
  
  delay(refrest_rate);
}
