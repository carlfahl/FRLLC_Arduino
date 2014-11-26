/*
  Blink
 Turns on an LED on for one second, then off for one second, repeatedly.
 
 This example code is in the public domain.
 */

// Includes
#include <avr/power.h>
#include <avr/sleep.h>


// Pin 13 has an LED connected on most Arduino boards.
// give it a name:
int led1 = 13;

const int sleepLimit = 1;
volatile int sleepTimes = 0;
volatile int timer1_flag = 0;

// the setup routine runs once when you press reset:
void setup() {
  // initialize Timer1
  cli();         // disable global interrupts
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

  // enable global interrupts:
  sei();

  pinMode(led1, OUTPUT);     
  // Serial is the USB connection to host computer.
  Serial.begin(9600);
  // On the Leonardo a sperate serial is availabe on PINs 0 and 1 - Serial 1
}

ISR(TIMER1_OVF_vect)
{
  Serial.println("got an interrupt");
  sleepTimes++;
  if (sleepTimes == sleepLimit)
  {
    if(timer1_flag == 0)
    {
      Serial.println("setting the timer1 flag");
      timer1_flag = 1;
    }
    Serial.println("Hello from ISR.  It's too early, I just woke up.");
    sleepTimes = 0; 
  }
}

void sleepNow ()
{
  Serial.println("going to sleep");
  /* The 5 different modes are:
   *     SLEEP_MODE_IDLE         -the least power savings 
   *     SLEEP_MODE_ADC
   *     SLEEP_MODE_PWR_SAVE
   *     SLEEP_MODE_STANDBY
   *     SLEEP_MODE_PWR_DOWN     -the most power savings
   */

  // sleep mode is set here
  // only SLEEP_MODE_IDLE will work with Timer1
  set_sleep_mode(SLEEP_MODE_IDLE);

  power_adc_disable();
  power_spi_disable();
  power_timer0_disable();
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

// the loop routine runs over and over again forever:
void loop() {
  if (timer1_flag == 1)
  {
    timer1_flag = 0;
    digitalWrite(led1, !digitalRead(led1));
  }
  sleepNow();
}

