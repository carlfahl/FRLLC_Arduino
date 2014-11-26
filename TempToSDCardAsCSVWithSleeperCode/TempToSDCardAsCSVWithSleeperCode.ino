/*
Read temperature and 
 Save as .CSV file
 On an SD card
 With sleeper code
 */

//Calling the sleeper function
#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/sleep.h>

// variable to store a request for sleep
int sleepStatus = 0;

//Calling the SD Card function
#include <SD.h>

//MUST ALWAYS SET CS PIN
const int CS_PIN = 10;

//This is the pin the SD card
//Will draw power from
//So set it high
const int POW_PIN = 8;

const int awakePin = 
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

  //Display a header in the .csv file
  File dataFile = SD.open("log.csv", FILE_WRITE);
  dataFile.print("Time");
  dataFile.print(",");
  dataFile.print("Temperature (F)");
  dataFile.print(",");
  dataFile.print("Temperature (C)");
  dataFile.print(",");
  dataFile.println("Temperature (K)");

  //Display a header on the Serial Monitor
  Serial.print("Time");
  Serial.print("    ");
  Serial.print("Temperature(F)");
  Serial.print("  ");
  Serial.print("Temperature(C)");
  Serial.print("  ");
  Serial.println("Temperature(K)");
}

void sleepNow()
{
  /* Now is the time to set the sleep mode.
   * In the avr/sleep.h file, the call names of these sleep modus are to be found:
   *
   * The 5 different modes are:
   *     SLEEP_MODE_IDLE         -the least power savings 
   *     SLEEP_MODE_ADC
   *     SLEEP_MODE_PWR_SAVE
   *     SLEEP_MODE_STANDBY
   *     SLEEP_MODE_PWR_DOWN     -the most power savings
   */

  // sleep mode is set here
  set_sleep_mode(SLEEP_MODE_STANDBY);

  // enables the sleep bit in the mcucr register
  // so sleep is possible. just a safety pin 
  sleep_enable();          

  power_adc_disable();
  power_spi_disable();
  power_timer0_disable();
  power_timer1_disable();
  power_timer2_disable();
  power_twi_disable();

  //actually putting device to sleep
  sleep_mode();


  // THE PROGRAM CONTINUES FROM HERE AFTER WAKING UP
  // first thing after waking from sleep:
  sleep_disable();

  // disable sleep...
  power_all_enable();
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

    delay(30000);
    
    //Putting the sensor to sleep after collecting
    //5 minutes of data
    if (timestamp >= 300000)
    {
      Serial.println("Timer: Entering Sleep Mode");
      //Need a delay so an error does not appear
      delay(100);
      timestamp--;
      digitalWrite(awakePin, Low);
      delay(100);
      digitalWrite(awakePin, HIGH);
    }
    else
    {
      timestamp=0;
    }
    }
}

