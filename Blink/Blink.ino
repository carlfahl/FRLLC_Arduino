/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.
 
  This example code is in the public domain.
 */
 
// Pin 13 has an LED connected on most Arduino boards.
// give it a name:
int led1 = 13;
int led2 = 7;
int spc = 5;
int potPin = 0;
int aval = 0;
int dtime = 500;
int tmp = 0;
int inByte;
int inByte1;
int numInBytes = 0;
int count = 0;
int numAvg = 10;

// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output.
  pinMode(led1, OUTPUT);     
  pinMode(led1, OUTPUT);
  // Serial is the USB connection to host computer.
  Serial.begin(9600);
  // On the Leonardo a sperate serial is availabe on PINs 0 and 1 - Serial 1
  Serial1.begin(9600);
}

// the loop routine runs over and over again forever:
void loop() {
  while(true)
  {
    while (Serial1.available() > 0)
    {
      inByte1 = Serial1.read();
      Serial.print("I received: ");
      //Serial1.println(inByte1);
      Serial.println(inByte1);
    }
    if (Serial.available() > 0)
    {
      numInBytes++;
      inByte = Serial.read();
      Serial.print("I received: ");
      Serial.println(inByte);
      if(isDigit(inByte))
      {
        //From Arduino Cookbook Second Edition
        tmp = (tmp*10) + (inByte - '0');
      }
      else if(inByte == 92)
      {
        dtime = tmp;
        tmp = 0;
        Serial.print("I received: ");
        Serial.println(dtime);
      }
    }
    else
    {
      break;
    }
  }
  for (int i=0; i<numAvg; i++)
  {
    aval += analogRead(potPin);
    delay(dtime/numAvg);
  }
  aval = aval/numAvg;
  //Serial.println(aval);
  //Serial1.println(aval);
  //dtime = 1000 - aval;
  aval = 0;
  digitalWrite(led1, HIGH);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(led2, LOW);
  digitalWrite(spc,LOW);
  delay(dtime);               // wait for a second
  digitalWrite(led1, LOW);    // turn the LED off by making the voltage LOW
  digitalWrite(led2, HIGH);
  digitalWrite(spc,HIGH);
  delay(dtime);               // wait for a second
  count++;
  if (count % 100 == 0)
  {
    //Serial.println("Ran a multiple of 100 times");
    //Serial1.println("Ran a multiple of 100 times");
    count = 0;
  }
}
