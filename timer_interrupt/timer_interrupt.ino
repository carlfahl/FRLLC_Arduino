/*
 * Sketch for testing sleep mode with wake up on timer.
 * Donal Morrissey - 2011.
 *
 */
#include <avr/sleep.h>
#include <avr/power.h>

#define LED_PIN (13)

volatile int f_timer=0;



/***************************************************
 *  Name:        ISR(TIMER1_OVF_vect)
 *
 *  Returns:     Nothing.
 *
 *  Parameters:  None.
 *
 *  Description: Timer1 Overflow interrupt.
 *
 ***************************************************/
ISR(TIMER1_OVF_vect)
{
  /* set the flag. */
   if(f_timer == 0)
   {
     Serial.println("setting the timer1 flag");
     f_timer = 1;
     Serial.println("waking up");
     sleep_disable();
     Serial.println("turning on all power");
     power_all_enable();
   }
}


/***************************************************
 *  Name:        enterSleep
 *
 *  Returns:     Nothing.
 *
 *  Parameters:  None.
 *
 *  Description: Enters the arduino into sleep mode.
 *
 ***************************************************/
void enterSleep(void)
{
  set_sleep_mode(SLEEP_MODE_IDLE);

  /* Disable all of the unused peripherals. This will reduce power
   * consumption further and, more importantly, some of these
   * peripherals may generate interrupts that will wake our Arduino from
   * sleep!
   */
  Serial.println("turning off power");
  power_adc_disable();
  power_spi_disable();
  power_timer0_disable();
  power_timer2_disable();
  power_twi_disable();  

  /* Now enter sleep mode. */
  Serial.println("going to sleep");
  sleep_mode();
  // Some commands will occur after the sleep_mode command
  // because it takes some time to happen.
  Serial.println("Takes some time to go to sleep.");
  Serial.println("still trying to sleep.");
  delay(500);
  // will resume here before going to the loop.
  Serial.println("Really!  Still running.");
  delay(500);
}



/***************************************************
 *  Name:        setup
 *
 *  Returns:     Nothing.
 *
 *  Parameters:  None.
 *
 *  Description: Setup for the serial comms and the
 *                timer. 
 *
 ***************************************************/
void setup()
{
  Serial.begin(9600);
  
  /* Don't forget to configure the pin! */
  pinMode(LED_PIN, OUTPUT);

  /*** Configure the timer.***/
  
  /* Normal timer operation.*/
  TCCR1A = 0x00; 
  
  /* Clear the timer counter register.
   * You can pre-load this register with a value in order to 
   * reduce the timeout period, say if you wanted to wake up
   * ever 4.0 seconds exactly.
   */
  TCNT1=0x0000; 
  
  /* Configure the prescaler for 1:1024, giving us a 
   * timeout of 4.09 seconds.
   */
  TCCR1B = 0x05;
  
  /* Enable the timer overlow interrupt. */
  TIMSK1=0x01;
}



/***************************************************
 *  Name:        enterSleep
 *
 *  Returns:     Nothing.
 *
 *  Parameters:  None.
 *
 *  Description: Main application loop.
 *
 ***************************************************/
void loop()
{
  if(f_timer==1)
  {
    f_timer = 0;
    /* Toggle the LED */
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    Serial.println("this happens after sleep fun resumes.");
    
    /* Re-enter sleep mode. */
    enterSleep();
  }
}
