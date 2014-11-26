/*
 * SaveTempInterruptSleep
 *
 * Read temperature and 
 * Save as .CSV file
 * On an SD card
 * With sleeper code
 * 
 * Tim Sorey*, Jennifer McClure*, and Carl A. Fahlstrom^
 *
 * *Central Washington University
 * ^Fahlstrom Research LLC
 *
 * Read of temperature and save to SD by Jenifer McClure and Tim Sorey - 2014.
 * Working version of sleep and logic to switch timers - Carl Fahlstrom - 2014.
 * use of timer1_flag from Donal Morrissey - 2011.
 * setup of timer1 from https://arduinodiy.wordpress.com/2012/02/28/timer-interrupts/
 * setup of watchdog timer from http://donalmorrissey.blogspot.com/2010/04/sleeping-arduino-part-5-wake-up-via.html
 */

// Includes
#include <avr/power.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <SD.h>

// Choose which internal timer will be used to wake the device 
// from sleep.
// Only one of these can be uncommented.
#define USE_WDT;        // Use the watchdog timer.
//#define USE_TIMER1;       // Use timer1.

// Pin 13 has an LED connected on most Arduino boards.
// give it a name:
int led1 = 13;

// Define the number of sleep cycles before data collection
// timer1 allows about 4s of sleep per cycle.
// WDT could be used for 8s sleep cycles.  Longer sleep cycles
// require an external trigger for wakeup.
const int sleepLimit = 1;
volatile int sleepTimes = 0;

volatile int timer_flag = 0;

// Set a number of samples to collect between sleep cycles.  If sampling will take more then
// 4s, the interrupt flag should be disabled until done.  May have to also disable the WDT if using it.
// 
int num_collect = 5;

// Select a delay between sample collections
int collect_delay = 100;

//MUST ALWAYS SET CS PIN
const int CS_PIN = 10;

//This is the pin the SD card
//Will draw power from
//So set it high
const int POW_PIN = 8;

//Temperature sensor on analog pin 0
int TEMP = 0;

// the setup routine runs once when you press reset:
void setup() {
  // initialize Timer1
  cli();         // disable global interrupts
  #ifdef USE_TIMER1
    const char* timer_st = "Using Timer1";
    TCCR1A = 0;    // set entire TCCR1A register to 0
    TCCR1B = 0;    // set entire TCCR1B register to 0 
    // enable Timer1 overflow interrupt:
    TIMSK1 |= (1 << TOIE1); //Atmega8 has no TIMSK1 but a TIMSK register

    // Set prescaling
    TCCR1B |= (1 << CS10); // Sets bit CS10 in TCCR1B
    TCCR1B |= (1 << CS12); // Sets bit CS12 in TCCR1B
    // This is achieved by shifting binary 1 (0b00000001)
    // to the left by CS10 bits. This is then bitwise
    // OR-ed into the current value of TCCR1B, which effectively set
    // this one bit high. Similar: TCCR1B |= _BV(CS10);
  #endif
  
  #ifdef USE_WDT
  const char* timer_st = "Using Watchdog timer";
  /*** Setup the WDT ***/
  
  /* Clear the reset flag. */
  MCUSR &= ~(1<<WDRF);
  
  /* In order to change WDE or the prescaler, we need to
   * set WDCE (This will allow updates for 4 clock cycles).
   */
  WDTCSR |= (1<<WDCE) | (1<<WDE);

  /* set new watchdog timeout prescaler value */
  WDTCSR = 1<<WDP0 | 1<<WDP3; /* 8.0 seconds */
  
  /* Enable the WD interrupt (note no reset). */
  WDTCSR |= _BV(WDIE);
  #endif
  
  // enable global interrupts:
  sei();
  
  pinMode(led1, OUTPUT);     
  // Serial is the USB connection to host computer.
  Serial.begin(9600);
  // On the Leonardo a sperate serial is availabe on PINs 0 and 1 - Serial 1
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
  else
  {
    Serial.println("Card Ready");
  }
  
  //Display a header in the .csv file
  File dataFile = SD.open("log.csv", FILE_WRITE);
  
  if (dataFile)
  {
    dataFile.println("New Data Log");
    dataFile.print("Time (ms)");
    dataFile.print(",");
    dataFile.print("Temperature (F)");
    dataFile.print(",");
    dataFile.print("Temperature (C)");
    dataFile.print(",");
    dataFile.println("Temperature (K)");
  }
  
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

#ifdef USE_TIMER1
ISR(TIMER1_OVF_vect)
#endif
#ifdef USE_WDT
ISR(WDT_vect)
#endif
{
  if(timer_flag == 0)
   {
     Serial.println("setting the timer flag");
     timer_flag = 1;
     Serial.println("waking up");
     sleep_disable();
     Serial.println("turning on all power");
     power_all_enable();
   }
   else
   {
     Serial.println("Timer overrun.");
   }
}




void sleepNow ()
{
  /* The 5 different modes are:
  *     SLEEP_MODE_IDLE         -the least power savings 
  *     SLEEP_MODE_ADC
  *     SLEEP_MODE_PWR_SAVE
  *     SLEEP_MODE_STANDBY
  *     SLEEP_MODE_PWR_DOWN     -the most power savings
  */

  // sleep mode is set here
  // only SLEEP_MODE_IDLE works with timer 1
  #ifdef USE_TIMER1
  set_sleep_mode(SLEEP_MODE_IDLE);
  #endif
  #ifdef USE_WDT
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  #endif
  Serial.println("turning off power");
  power_adc_disable();
  power_spi_disable();
  power_timer0_disable();
  #ifdef USE_WDT
  power_timer1_disable();
  #endif
  power_timer2_disable();
  power_twi_disable();

  //actually putting device to sleep
  Serial.println("going to sleep");
  sleep_mode();
}

// the loop routine runs over and over again forever:
void loop() 
{
  if (timer_flag == 1)
  {
    Serial.println("clearing the timer1 flag");
    timer_flag = 0;
    sleepTimes++;
    if (sleepTimes == sleepLimit)
    {
      if ((num_collect * collect_delay) > 3000)
      {
        // Disable global interrupts
        cli();
      }
      for (int i=0; i<num_collect;i++)
      {
        sleepTimes = 0;
        Serial.println("collecting data");
        digitalWrite(led1, !digitalRead(led1));
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
        
        delay(collect_delay);
        
      }
      if ((num_collect * collect_delay) > 3000)
      {
        // Enable global interrupts
        sei();
      }
    }
    
    sleepNow();
    
  }
}

