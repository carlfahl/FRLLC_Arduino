/*
Read temperature and pH
Save as .CSV file
On an SD card
*/
#include <avr/power.h>
#include <SD.h>
#include <SoftwareSerial.h>
#define rx 2
#define tx 3

SoftwareSerial myserial(rx, tx);

//MUST ALWAYS SET CS PIN
const int CS_PIN = 10;

//Defining the variables
char ph_data[20];
char computerdata[20];
byte received_from_computer=0;
byte received_from_sensor=0;
byte arduino_only=0;

byte startup=0;
float ph=0;
byte string_received=0;

//This is the pin the SD card
//Will draw power from
//So set it high
const int POW_PIN = 8;

//Temperature sensor on analog pin 0
int TEMP = 0;

void setup()
{
  Serial.begin(9600);
  Serial.println("Initializing Card");
  
  //Set CS Pin as an output
  pinMode(CS_PIN, OUTPUT);
  
  //Set the power high
  pinMode(POW_PIN, OUTPUT);
  digitalWrite(POW_PIN, HIGH);
  
  //Has the card been initialized?
  if (!SD.begin(CS_PIN))
  {
    Serial.println("Card Failure");
    return;
  }
  
  Serial.println("Card Ready");
  
  long timestamp = millis();
  
  
  if (timestamp <=100)
  {
  //Display a header in the .csv file
  File dataFile = SD.open("log.csv", FILE_WRITE);
  dataFile.println("New Data Log");
  dataFile.print("Time (ms)");
  dataFile.print(",");
  dataFile.print("Temperature (F)");
  dataFile.print(",");
  dataFile.print("Temperature (C)");
  dataFile.print(",");
  dataFile.println("Temperature (K)");
  
  //Display a header on the Serial Monitor
  Serial.println("New Data Log");
  Serial.print("Time(ms)");
  Serial.print("    ");
  Serial.print("Temperature(F)");
  Serial.print("  ");
  Serial.print("Temperature(C)");
  Serial.print("  ");
  Serial.println("Temperature(K)");
  }
}

void serialEvent()
{
  if(arduino_only!=1)
  {
    received_from_computer=Serial.readBytesUntil(13,computerdata,20);
    
    computerdata[received_from_computer]=0;
    
    myserial.print(computerdata);
    
    myserial.print('\r');
  }
}

void loop()
{
  long timestamp = millis();
  
  //Open a file and write to it
  File dataFile = SD.open("log.csv", FILE_WRITE);
  
  if (dataFile)
  {
    //Read the temperature sensor in bits
    int BIT = analogRead(TEMP);
    //Convert bits into farenheit
    float TempF =(BIT / 1024.0) * (5000 / 10);
    //Convert farenheit into celsius
    float TempC = ( (TempF - 32) * 5) / 9;
    //conver celsius into kelvin
    float TempK = 273 + TempC;
    
    //Displaying the data in log.csv
    dataFile.print(timestamp);
    dataFile.print(",");
    dataFile.print(TempF);
    dataFile.print(",");
    dataFile.print(TempC);
    dataFile.print(",");
    dataFile.println(TempK);

    //Data isnt written to log.csv
    //until the program is closed
    dataFile.close();
    
    //Displaying the data in the Serial Monitor
    Serial.print(timestamp);
    Serial.print("ms  ");
    Serial.print(TempF);
    Serial.print("F  ");
    Serial.print(TempC);
    Serial.print("C  ");
    Serial.print(TempK);
    Serial.println("K");
  }
  
  else
  {
    Serial.println("Couldn't open log.csv");
  }
  
    if(myserial.available()>0)
  {
    received_from_sensor=myserial.readBytesUntil(13,ph_data,20);
    
    ph_data[received_from_sensor]=0;
    
    string_received=1;
    
    Serial.println(ph_data);
  }
  
    if(arduino_only==1)
  {
    Arduino_Control();
  }
}

void Arduino_Control()
{
  if(startup==0)
  {
    myserial.print("e\r");
    delay(50);
    myserial.print("e\r");
    delay(50);
    
    startup=1;
  }
  
  delay(5000);
}
